#include "cutest/CuTest.h"
#include "alloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
    

void TestMyMalloc0(CuTest *tc) {
  	setup_heap();
  	void *p1 = my_malloc(0);
  	CuAssertPtrEquals(tc, NULL, p1);
  	teardown_heap();
}


void TestMyMalloc1(CuTest *tc) {
	setup_heap();
	void* p1 = my_malloc(1);
        print_heap();
	CuAssertPtrNotNull(tc, p1);
	CuAssertPtrEquals(tc, current_free_list, p1 + 4);
	CuAssertPtrNotNull(tc, p1);
	int result = *(int*)(p1 - 4);
	CuAssertIntEquals(tc, 5, result);

	void* p2 = my_malloc(18);
	print_heap();
	CuAssertPtrNotNull(tc, p2);
	CuAssertIntEquals(tc, 21, *(int*)(p2 - 4));
	CuAssertPtrEquals(tc, current_free_list, p2 + 20);

	int value = my_free(p2);
	print_heap();
	CuAssertIntEquals(tc, 1, value);
	CuAssertPtrEquals(tc, current_free_list, heap_ptr + 8);
	CuAssertIntEquals(tc, 20, *(int*)(heap_ptr + 8));
	void* next = *(void**)(heap_ptr + 12);
	CuAssertPtrEquals(tc, heap_ptr + 32, next);

	void* p3 = my_malloc(8);
	print_heap();
	CuAssertPtrNotNull(tc, p3);
	CuAssertIntEquals(tc, 9, *(int*)(p3 - 4));
	CuAssertPtrEquals(tc, current_free_list, p3 + 8);
	CuAssertIntEquals(tc, 8, *(int*)(p3 + 8));
	void* next1 = *(void**)(p3 + 12);
	CuAssertPtrEquals(tc, p3 + 20, next1);

	void* p4 = my_malloc(12);
	print_heap();
	CuAssertPtrNotNull(tc, p4);
	CuAssertIntEquals(tc, 13, *(int*)(p4 - 4));
	CuAssertIntEquals(tc, 976, *(int*)(p4 + 12));
	CuAssertPtrEquals(tc, current_free_list, p4 - 16);
	void* next2 = *(void**)(p4 - 12);
	CuAssertPtrEquals(tc, p4 + 12, next2);
	

	void* p5 = my_malloc(3);
	print_heap();
	CuAssertPtrNotNull(tc, p5);
	CuAssertIntEquals(tc, 9, *(int*)(p5 - 4));
	CuAssertPtrEquals(tc, heap_ptr + 24, p5);
	CuAssertPtrEquals(tc, current_free_list, heap_ptr + 48);
	teardown_heap();
}


void TestMyFree1(CuTest *tc) {
	//set heap as the one in the previous test
	setup_heap();
	void* p1 = my_malloc(1);
	void* p2 = my_malloc(18);
	my_free(p2);
	void* p3 = my_malloc(8);
	void* p4 = my_malloc(12);
	void* p5 = my_malloc(3);
	print_heap();

	int result = my_free(p5 + 4);
	CuAssertIntEquals(tc, 0, result);

	int result1 = my_free(current_free_list + 16);
	CuAssertIntEquals(tc, 0, result1);
	
	int val = my_free(p1);
	print_heap();
	CuAssertIntEquals(tc, 1, val);
	CuAssertPtrEquals(tc, current_free_list, heap_ptr);
	CuAssertIntEquals(tc, 4, *(int*)current_free_list);
	void* next0 = *(void**)(current_free_list + 4);
	CuAssertPtrEquals(tc, heap_ptr + 48, next0);
	
	int val1 = my_free(p4);
	print_heap();
	CuAssertIntEquals(tc, 1, val1);
	CuAssertPtrEquals(tc, current_free_list, heap_ptr + 32);
	CuAssertIntEquals(tc, 12, *(int*)(heap_ptr + 32));
	void* next1 = *(void**)(current_free_list + 4);
	CuAssertPtrEquals(tc, heap_ptr, next1);
	void* next2 = *(void**)(heap_ptr + 4);
	CuAssertPtrEquals(tc, current_free_list + 16, next2);

	int val2 = my_free(NULL);
	CuAssertIntEquals(tc, 0, val2);

	int val0 = my_free(current_free_list);
	CuAssertIntEquals(tc, 0, val0);

	int val3 = my_free(current_free_list + 4);
	CuAssertIntEquals(tc, 0, val3);

	int val4 = my_free(p3);
	print_heap();
	CuAssertIntEquals(tc, 1, val4);
	CuAssertPtrEquals(tc, current_free_list, heap_ptr + 8);
	CuAssertIntEquals(tc, 8, *(int*)current_free_list);

	void* next3 = *(void**)(heap_ptr + 12);
	CuAssertPtrEquals(tc, heap_ptr + 32, next3);
	int num = *(int*)(heap_ptr + 32);
	CuAssertIntEquals(tc, 12, num);

	void* next4 = *(void**)(heap_ptr + 36);
	CuAssertPtrEquals(tc, heap_ptr, next4);
	int num1 = *(int*)(heap_ptr);
	CuAssertIntEquals(tc, 4, num1);

	void* next5 = *(void**)(heap_ptr + 4);
	CuAssertPtrEquals(tc, heap_ptr + 48, next5);
	int num2 = *(int*)(heap_ptr + 48);
	CuAssertIntEquals(tc, 976, num2);
	teardown_heap();
}


void TestFreeSpace(CuTest *tc) {
	setup_heap();
	int total0 = free_space();
	CuAssertIntEquals(tc, 1024, total0);

	void* p1 = my_malloc(1);
	void* p2 = my_malloc(18);
	my_free(p2);
	
	int total = free_space();
	CuAssertIntEquals(tc, 1012, total);

	void* p3 = my_malloc(8);
	void* p4 = my_malloc(12);
	void* p5 = my_malloc(3);
	int total1 = free_space();
	CuAssertIntEquals(tc, 976, total1);
	my_free(p1);
	my_free(p4);
	my_free(p3);
	int total2 = free_space();
	CuAssertIntEquals(tc, 1000, total2);

	teardown_heap();

}


void TestLiveData(CuTest *tc) {
	setup_heap();
	int sum0 = live_data();
	CuAssertIntEquals(tc, 0, sum0);
	
	void* p1 = my_malloc(1);
	void* p2 = my_malloc(18);
	int sum = live_data();
	CuAssertIntEquals(tc, 24, sum);

	my_free(p2);
	int sum1 = live_data();
	CuAssertIntEquals(tc, 4, sum1);
	
	void* p3 = my_malloc(8);
	int sum2 = live_data();
	CuAssertIntEquals(tc, 12, sum2);

	void* p4 = my_malloc(12);
	void* p5 = my_malloc(3);
	int sum3 = live_data();
	CuAssertIntEquals(tc, 32, sum3);

	my_free(p1);
	my_free(p4);
	my_free(p3);
	int sum4 = live_data();
	CuAssertIntEquals(tc, 8, sum4);
	teardown_heap();
}


void TestConsolidate(CuTest *tc) {
	setup_heap();
	consolidate();
	CuAssertPtrNotNull(tc, current_free_list);
	CuAssertPtrEquals(tc, current_free_list, heap_ptr);
	CuAssertPtrEquals(tc, NULL, *(void**)(current_free_list + 4));

	void* p1 = my_malloc(1);
	void* p2 = my_malloc(18);
	my_free(p2);
	
	consolidate();
	CuAssertPtrNotNull(tc, current_free_list);
	CuAssertPtrEquals(tc, current_free_list, heap_ptr + 8);
	CuAssertIntEquals(tc, 1016, *(int*)current_free_list);
	CuAssertPtrEquals(tc, NULL, *(void**)(current_free_list + 4));

	my_free(p1);
	consolidate();
	CuAssertPtrEquals(tc, current_free_list, heap_ptr);
	CuAssertPtrEquals(tc, NULL, *(void**)(current_free_list + 4));

	void* p3 = my_malloc(1024);
	CuAssertPtrEquals(tc, NULL, current_free_list);


	consolidate();
	CuAssertPtrEquals(tc, NULL, current_free_list);
	
	void* p4 = my_malloc(6);
	CuAssertIntEquals(tc, 1025, *(int*)heap_ptr);

	my_free(p3);
	void* p5 = my_malloc(1);
	void* p6 = my_malloc(18);
	my_free(p6);
	void* p7 = my_malloc(8);
	void* p8 = my_malloc(12);
	void* p9 = my_malloc(3);
	my_free(p5);
	my_free(p8);
	my_free(p7);
	consolidate();
	CuAssertPtrNotNull(tc, current_free_list);
	CuAssertPtrEquals(tc, current_free_list, heap_ptr);
	CuAssertIntEquals(tc, 16, *(int*)current_free_list);
	CuAssertPtrEquals(tc, heap_ptr + 32, *(void**)(current_free_list + 4));
	CuAssertPtrEquals(tc, NULL, *(void**)(heap_ptr + 36)); 

	void* p10 = my_malloc(1);
	CuAssertPtrNotNull(tc, current_free_list);
	CuAssertPtrEquals(tc, current_free_list, heap_ptr + 8);
	CuAssertIntEquals(tc, 5, *(int*)heap_ptr);
	teardown_heap();

}


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



void TestMyFreeOnFreeEntry(CuTest * tc){
  	setup_heap();
  	int result = my_free(current_free_list); 
 	CuAssertIntEquals(tc, 0,result);
  	teardown_heap();
}


/*
 * Note that as you add new testing functions, you need to add them to the list
 * below using SUITE_ADD_TEST. Otherwise they will not run.
 */
CuSuite* StrUtilGetSuite() {
  	CuSuite* suite = CuSuiteNew();
  	SUITE_ADD_TEST(suite, TestMyMalloc0);
  	SUITE_ADD_TEST(suite, TestMyFreeOnFreeEntry);
  	SUITE_ADD_TEST(suite, TestMyMalloc1);
  	SUITE_ADD_TEST(suite, TestMyFree1);
  	SUITE_ADD_TEST(suite, TestFreeSpace);
  	SUITE_ADD_TEST(suite, TestLiveData);
  	SUITE_ADD_TEST(suite, TestConsolidate);
 	SUITE_ADD_TEST(suite, Readme_Q1);
  	SUITE_ADD_TEST(suite, Readme_Q2);
  return suite;
}
