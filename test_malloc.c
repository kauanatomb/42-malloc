#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>
#include "malloc.h"

#define TEST(name) printf("\n========== %s ==========\n", name)
#define ASSERT(cond, msg) do { if (!(cond)) { printf("❌ FAIL: %s\n", msg); return (1); } else printf("✅ PASS: %s\n", msg); } while (0)

int test_basic_malloc(void)
{
	TEST("Basic malloc");
	void *a = malloc(64);
	ASSERT(a != NULL, "malloc returned NULL");
	memset(a, 42, 64);
	free(a);
	return (0);
}

int test_free_null(void)
{
	TEST("Free NULL");
	free(NULL);
	ASSERT(1, "free(NULL) didn't crash");
	return (0);
}

int test_reuse(void)
{
	TEST("Block reuse after free");
	void *a = malloc(100);
	void *addr_a = a;
	free(a);
	
	void *b = malloc(100);
	ASSERT(b == addr_a, "block reused after free");
	free(b);
	return (0);
}

int test_split(void)
{
	TEST("Block splitting");
	void *a = malloc(500);
	void *b = malloc(100);
	ASSERT(a != NULL && b != NULL, "split allocation succeeded");
	
	memset(a, 1, 500);
	memset(b, 2, 100);
	free(a);
	free(b);
	return (0);
}

int test_realloc_grow(void)
{
	TEST("Realloc grow");
	void *a = malloc(100);
	memset(a, 42, 100);
	
	void *b = realloc(a, 200);
	ASSERT(b != NULL, "realloc grow succeeded");
	ASSERT(((char *)b)[50] == 42, "data preserved on grow");
	
	free(b);
	return (0);
}

int test_realloc_shrink(void)
{
	TEST("Realloc shrink");
	void *a = malloc(500);
	memset(a, 99, 500);
	
	void *b = realloc(a, 100);
	ASSERT(b != NULL, "realloc shrink succeeded");
	ASSERT(((char *)b)[50] == 99, "data preserved on shrink");
	
	free(b);
	return (0);
}

int test_realloc_null(void)
{
	TEST("Realloc NULL pointer");
	void *a = realloc(NULL, 100);
	ASSERT(a != NULL, "realloc(NULL, size) acts like malloc");
	free(a);
	return (0);
}

int test_many_allocations(void)
{
	TEST("Many allocations (100)");
	void *ptrs[100];
	
	for (int i = 0; i < 100; i++) {
		ptrs[i] = malloc(64 + i);
		ASSERT(ptrs[i] != NULL, "allocation succeeded");
		memset(ptrs[i], i, 64 + i);
	}
	
	for (int i = 0; i < 100; i++)
		free(ptrs[i]);
	
	return (0);
}

int test_large_allocation(void)
{
	TEST("Large allocation (LARGE zone)");
	void *a = malloc(100000);
	void *b = malloc(200000);
	
	ASSERT(a != NULL && b != NULL, "large allocations succeeded");
	memset(a, 1, 100000);
	memset(b, 2, 200000);
	
	free(a);
	free(b);
	return (0);
}

int test_zone_display(void)
{
	TEST("Show allocation memory");
	void *tiny = malloc(50);
	void *small = malloc(500);
	void *large = malloc(100000);
	
	// Call show_alloc_mem dynamically by dlsym
	typedef void (*show_alloc_mem_t)(void);
	show_alloc_mem_t show_alloc_mem_func = (show_alloc_mem_t)dlsym(RTLD_DEFAULT, "show_alloc_mem");
	if (show_alloc_mem_func)
		show_alloc_mem_func();
	else
		printf("(show_alloc_mem not available)\n");
	
	free(tiny);
	free(small);
	free(large);
	return (0);
}

void	*thread_malloc(void *arg)
{
	int id = (intptr_t)arg;
	void *ptrs[10];
	
	for (int i = 0; i < 10; i++) {
		ptrs[i] = malloc(100 + (id * 10) + i);
		if (!ptrs[i])
			return NULL;
		memset(ptrs[i], id, 100 + (id * 10) + i);
	}
	
	for (int i = 0; i < 10; i++)
		free(ptrs[i]);
	
	return (void *)1;
}

int test_thread_safety(void)
{
	TEST("Thread safety");
	pthread_t threads[4];
	
	for (int i = 0; i < 4; i++)
		pthread_create(&threads[i], NULL, thread_malloc, (void *)(intptr_t)i);
	
	for (int i = 0; i < 4; i++) {
		void *ret;
		pthread_join(threads[i], &ret);
		ASSERT(ret != NULL, "thread completed successfully");
	}
	
	return (0);
}

int test_tiny_zone(void)
{
	TEST("TINY zone allocation (< 64 bytes)");
	void *ptrs[10];
	
	for (int i = 0; i < 10; i++) {
		ptrs[i] = malloc(32);
		ASSERT(ptrs[i] != NULL, "tiny allocation succeeded");
	}
	
	for (int i = 0; i < 10; i++)
		free(ptrs[i]);
	
	return (0);
}

int test_small_zone(void)
{
	TEST("SMALL zone allocation (64-1024 bytes)");
	void *ptrs[10];
	
	for (int i = 0; i < 10; i++) {
		ptrs[i] = malloc(256);
		ASSERT(ptrs[i] != NULL, "small allocation succeeded");
	}
	
	for (int i = 0; i < 10; i++)
		free(ptrs[i]);
	
	return (0);
}

int main(void)
{
	int failed = 0;

	failed += test_basic_malloc();
	failed += test_free_null();
	failed += test_reuse();
	failed += test_split();
	failed += test_realloc_grow();
	failed += test_realloc_shrink();
	failed += test_realloc_null();
	failed += test_tiny_zone();
	failed += test_small_zone();
	failed += test_many_allocations();
	failed += test_large_allocation();
	failed += test_zone_display();
	failed += test_thread_safety();
    check_leaks();
	printf("\n");
	printf("╔════════════════════════════════════════════════════════════╗\n");
	if (failed == 0)
		printf("║              ✅ ALL TESTS PASSED                          ║\n");
	else
		printf("║              ❌ %d TEST(S) FAILED                        ║\n", failed);
	printf("╚════════════════════════════════════════════════════════════╝\n");
	printf("\n");

	return (failed);
}
