ReadMe: 
1.
void Readme_Q1(CuTest *tc) {
	setup_heap();
	void * array[128];
	int count = 0;
	while(count < 128) {
		void* p1 = my_malloc(2);
		array[count] = p1;
		count++;
	}
	print_heap();
	count = 0;
	while(count < 128) {
		my_free(array[count]);
		count++;
	}


	int space = free_space();
	printf("free space is %d\n",space);

	void* result = my_malloc(11);
	CuAssertIntEquals(tc, 8, *(int*)current_free_list);
	CuAssertPtrEquals(tc, NULL, result);
	teardown_heap();	
}

The test function here sets up a heap and creates a list of void pointer to 
store the address of allocated point. Now we allocate this heap and store
the addresses into the list. As the result, now the whole heap is seperated
and each byte inside is occupied. The size of each byte would be 5 except
the last byte, whose size is 9. Then according to the address stored in list,
we free all of them. Finally, since the biggest size of free byte in this heap
is 8, we cannot find an appropriate address for the input size 11. And here
the return value of free_space() is much larger than 11. Hence, result is 
NULL.  



2.
void Readme_Q2(CuTest *tc) {
	setup_heap();
	void* p1 = my_malloc(400);
	void* p2 = my_malloc(300);
	void* p3 = my_malloc(200);
	void* p4 = my_malloc(60);
	void* p5 = my_malloc(25);
	void* p6 = my_malloc(10);
	void* p7 = my_malloc(5);
	void* p8 = my_malloc(1);
	my_free(p1);
	my_free(p2);
	my_free(p3);
	my_free(p4);
	my_free(p5);
	my_free(p6);
	my_free(p7);
	my_free(p8);
	void* ptr1 = my_malloc(400);
	void* ptr2 = my_malloc(300);
	void* ptr3 = my_malloc(200);
	void* ptr4 = my_malloc(60);
	void* ptr5 = my_malloc(25);
	void* ptr6 = my_malloc(10);
	void* ptr7 = my_malloc(5);
	void* ptr8 = my_malloc(1);
	teardown_heap();
}

At first part, we malloced 8 points inside the heap with different size from
bigger to smaller. Hence the sequence of these occupied bytes are from
left to right respectively. Then we free these points in the same order as
before, which resulted in the current_free_list finally points to p8 - 4. 
Here the size of this free byte is only 4, and so when we try to malloc 
these sizes again in the same order, each time it has to keep checking 
the size of next free byte, and promote this checking to the end. However,
if we reverse the order of free list, each time these points with the size
from bigger to smaller would directly go to the current_free_list, the 1st
byte in free linked list. Hence, this test function achieves the requirement.
