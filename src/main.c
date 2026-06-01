#include "malloc.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ASSERT(cond, msg) \
    do { if (!(cond)) { printf("[FAIL] %s\n", msg); return 1; } } while (0)

#define TEST(name) \
    printf("\n--- %s ---\n", name)

int test_basic_malloc(void)
{
    TEST("basic malloc");

    void *a = malloc(64);

    ASSERT(a != NULL, "malloc returned NULL");

    memset(a, 42, 64);

    free(a);
    printf("[OK] basic malloc\n");
    return 0;
}

int test_free_reuse(void)
{
    TEST("free + reuse");

    void *a = malloc(64);
    free(a);

    void *b = malloc(64);

    ASSERT(b != NULL, "reuse failed");
    ASSERT(b == a, "block not reused");

    free(b);
    printf("[OK] reuse\n");
    return 0;
}

int test_split(void)
{
    TEST("split");

    void *a = malloc(200);
    void *b = malloc(64);

    ASSERT(a != NULL && b != NULL, "split allocation failed");

    free(a);
    free(b);
    printf("[OK] split\n");
    return 0;
}

int test_many_allocs(void)
{
    TEST("many allocs");

    void *ptrs[100];

    for (int i = 0; i < 100; i++)
    {
        ptrs[i] = malloc(64);
        ASSERT(ptrs[i] != NULL, "allocation failed");
        free(ptrs[i]);
    }

    printf("[OK] many allocs\n");
    return 0;
}

int test_large(void)
{
    TEST("large alloc");

    void *a = malloc(100000);
    void *b = malloc(200000);

    ASSERT(a != NULL && b != NULL, "large malloc failed");

    free(a);
    free(b);
    printf("[OK] large alloc\n");
    return 0;
}

int main(void)
{
    int res = 0;

    res |= test_basic_malloc();
    res |= test_free_reuse();
    res |= test_split();
    res |= test_many_allocs();
    res |= test_large();

    if (res == 0)
        printf("\nALL TESTS PASSED\n");
    else
        printf("\nSOME TESTS FAILED\n");
    check_leaks();
    return res;
}