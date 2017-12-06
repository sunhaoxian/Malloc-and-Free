#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

size_t HEAP_SIZE = 1028; // in bytes (e.g. 1kb of memory)
int WORD_SIZE = sizeof(int);

void* heap_ptr = NULL;
void* current_free_list = NULL;


void setup_heap() {
	// The mmap system call asks the operating system to please put 
	// HEAP_SIZE
	// memory into this process's address space. The resulting addresses
	// start at the very beginning of heap space (more or less). This is 
	// the
	// lowest-level interface to access memory without writing an 
	// operating
	// system yourself. It returns memory in fairly large chunks (usually
	// at least 1kb gets mapped at a time).
	heap_ptr = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, 
			MAP_ANON | MAP_SHARED, -1, 0);

	// Set the first word on the heap to hold the total size available.
	// See above for
	// a description of *(int*)ptr
	*(int*)heap_ptr = HEAP_SIZE - WORD_SIZE;
	*(void**)(heap_ptr+WORD_SIZE) = NULL;
	current_free_list = heap_ptr;
}


/*
 * Name: teardown_heap
 * Parameter: None
 * Function: This function is to tear the heap created in each tester 
 * 		function down.
 * Return: None
 */ 
void teardown_heap(){
  munmap(heap_ptr, HEAP_SIZE);
  heap_ptr = NULL;
  current_free_list = NULL;
}



/*
 * Name: my_malloc
 * Parameter: size - the size that needs to be allocated in heap
 * Purpose: This function is to see whether there exists a free byte large
 * 	enough to contain the the parameter size. If it has been found, change
 *	 the value inside that address into odd number.
 * Return: a void* representing the address of the newly created point
 */ 
void *my_malloc(size_t size){

	//edge case to check the range
	if (size <= 0) {
		return NULL;
	}

	if (size > HEAP_SIZE) {
		return NULL;
	}
	
	//make the size become the multiple of 4
	if (size % WORD_SIZE != 0) {
		size = size + (WORD_SIZE - (size % WORD_SIZE));
	}

	//enter the linked list of free bytes
	void* cur = current_free_list;
	void* old = NULL;

	//check the address of cur
	while(cur != NULL) {

		//get teh size of current free byte
		int cur_size = *(int*)cur;

		//if that byte is too small to store the input size
		//promote to next free-byte address
		if (cur_size < size) {
			old = cur;
			cur = *(void**)(cur + WORD_SIZE);
			continue;
		}

		//else, check the splitting and reusing cases
		else {

			//splitting case
			if (size + WORD_SIZE + WORD_SIZE <= cur_size) {

				//get the address of new free byte after 
				//splitting
				void* addressOfNewFree = cur + size + 
							WORD_SIZE;

				//if the first free byte in the list is used
				if (cur == current_free_list) {		

					//change current_free_list's address
					current_free_list = addressOfNewFree;
				}

				//otherwise, let the next pointer of last free
				//byte point to new free byte
				else {
					*(void**)(old + WORD_SIZE) = 
						addressOfNewFree;
				}

				//change the content of new free byte
				*(int*)addressOfNewFree = cur_size -
						      size - WORD_SIZE;

				//change the content of new next pointer
				*(void**)(addressOfNewFree + WORD_SIZE)=
						*(void**)(cur + WORD_SIZE);

				//set the content of original cur to odd 
				//number
				*(int*)cur = size;
				*(int*)cur |= 0x1;
				return cur + WORD_SIZE;
			}

			//reusing the whole free byte
			else {

				//add the content by 1
				*(int*)cur |= 0x1;

				//store the new address
				void* newAddress = *(void**)(cur + WORD_SIZE);

				//check if the 1st free byte is used
				if (cur == current_free_list) {

					//current_free_list points to the new
					//address
					current_free_list = newAddress;
				}

				//otherwise, change the content of last 
				//"next pointer"				
				else {
					*(void**)(old + WORD_SIZE) = 
						newAddress;
				}			
				return cur + WORD_SIZE;			
			}
		}
	}
	return NULL;
}



/*
 * Name: my_free
 * Parameter: ptr - a random address needed to be checked
 * Purpose: This function is to check whether the input address can be 
 * 	    freed from the existing heap.
 * Return: 1 - successful;  0 - failure
 */ 
int my_free(void *ptr){

	if(ptr == NULL) {
		return 0;
	}

	//chech the range of ptr
	if (ptr <= heap_ptr) {
		return 0;
	}

	if (ptr > heap_ptr + HEAP_SIZE) {
		return 0;
	}

	//see if the ptr is the multiple of 4
	if ((int)ptr % WORD_SIZE != 0) {
		return 0;
	}

	//check from the beginning
	void* check = heap_ptr;

	//keep checking
	while (1) {

		//get the size of each byte
		int size = *(int*)check;

		//see if the byte is occupied or not
		if (size % WORD_SIZE == 0) {
	
			//if unoccupied, promote to next byte
			check = check + size + WORD_SIZE;
		}

		//if it is occupied
		else {

			//if it is at the correct position
			if (ptr == check + WORD_SIZE) {
				break;
			}

			//if we have passed the desired address
			else if (ptr < check + WORD_SIZE) {
				return 0;
			}

			//if we passed, keep checking next byte
			else {
				check = check + (size - 1) + WORD_SIZE;
			}
		}
		
		//check the range of check pointer
		if (check >= heap_ptr + HEAP_SIZE) {
			return 0;
		}
	}

	//if it passes all the checkings, set the value of occupied byte
	//into a value of free byte
	*(int*)(ptr - WORD_SIZE) &= ~0x1;

	//make the next pointer point to last free byte	
	*(void**)ptr = current_free_list;

	//renew the current_free_list
	current_free_list = ptr - WORD_SIZE;
	return 1;
}



/*
 * Name: consolidate
 * Parameter: None
 * Purpose: consolidate all the adjacant free bytes inside the heap, and 
 * 	    renew the address of "current_free_list" and "next*" if necessary.
 * Return: None
 */ 
void consolidate(){

	//enter from the beginning of heap
	void* check = heap_ptr;
	void* oldValAddress = NULL;
	void* oldPtrAddress = NULL;
	void* newValAddress = NULL;
	void* newPtrAddress = NULL;

	//set the range of check variable
	while (check < heap_ptr + HEAP_SIZE) {

		//get the size of each byte
		int size = *(int*)check;

		//if it is free byte
		if (size % WORD_SIZE == 0) {

			//get the address of next byte
			newValAddress = check + size + WORD_SIZE;
	
			//store the address of current byte
			oldValAddress = check;
			oldPtrAddress = check + WORD_SIZE;

			//if the next bytes (one or more) are also free bytes
			//and that address does not exceed the range of heap
			while (*(int*)newValAddress % WORD_SIZE == 0 && 
				newValAddress < heap_ptr + HEAP_SIZE) {

				//renew the size of current free byte
				*(int*)oldValAddress += *(int*)newValAddress
							+ WORD_SIZE;

				//the next pointer's address of next free byte
				newPtrAddress = newValAddress + WORD_SIZE;

				//renew the content of current next pointer
				*(void**)oldPtrAddress = 
						*(void**)newPtrAddress;

				//promote to check the next byte of next byte
				newValAddress += *(int*)newValAddress +
						 WORD_SIZE;
			}

			//renew check to promote loop checking
			check = newValAddress;
		}

		//if it is occupied byte, skip it
		else {
			check = check + (size - 1) + WORD_SIZE;
		}
	}

	//entering the beginning of heap after consolidation
	void* freeAdd = heap_ptr;

	//reset current_free_list if it doesn't enter the loop below
	current_free_list = NULL;

	//the loop to reset current_free_list pointing to the 1st free byte 
	while (freeAdd < heap_ptr + HEAP_SIZE) {
		int tempVal = *(int*)freeAdd;
		if (tempVal % WORD_SIZE == 0) {
			current_free_list = freeAdd;
			break;
		}
		else {
			freeAdd += (tempVal - 1) + WORD_SIZE;
		}
	}
}



/*
 * Name: print_heap
 * Parameter: None
 * Purpose: print the whole heap
 * Return: None
 */ 
void print_heap(){
	int i;
	for (i = 0; i < HEAP_SIZE; i = i + 4) {
		printf("%10p:\t%d\t%#010x\n", heap_ptr + i, 
			*(int*)(heap_ptr + i), *(int*)(heap_ptr + i));
	}
}



/*
 * Name: free_space
 * Parameter: None
 * Purpose: This function is to get the total number of free space
 * Return: num - the total number of free space
 */ 
int free_space(){

	//enter the free list
	void* count = current_free_list;
	int num = 0;

	//adding the value store in each free-byte address
	while (count != NULL) {
		num += *(int*)count;
		count = *(void**)(count + WORD_SIZE);
	}
	return num;
}



/*
 * Name: live_data
 * Parameter: None
 * Purpose: This function is to calculate the total number of occupied bytes
 * 	    in the heap.
 * Return: count - the total number of occupied bytes
 */ 
int live_data() {

	//get the beginning of the heap_ptr
	void* data = heap_ptr;
	int count = 0;

	//see if the address of data is outside the range
	while (data < heap_ptr + HEAP_SIZE) {

		//store the number of bytes
		int size = *(int*)data;

		//if it is free byte, skip to next byte
		if (size % WORD_SIZE == 0) {
			data = data + size + WORD_SIZE;
		}

		//if it is occupied byte, plus count by (size - 1)
		//and then promote to check next byte
		else {
			count += (size - 1);
			data = data + (size - 1) + WORD_SIZE;
		}
	}
	return count;
}
