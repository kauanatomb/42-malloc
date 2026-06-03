#include "malloc.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ASSERT(cond, msg) \
    do { if (!(cond)) { printf("[FAIL] %s\n", msg); return 1; } } while (0)

#define TEST(name) \
    printf("\n--- %s ---\n", name)

// int test_basic_malloc(void)
// {
//     TEST("basic malloc");

//     void *a = malloc(64);

//     ASSERT(a != NULL, "malloc returned NULL");

//     memset(a, 42, 64);

//     free(a);
//     printf("[OK] basic malloc\n");
//     return 0;
// }

// int test_free_reuse(void)
// {
//     TEST("free + reuse");

//     void *a = malloc(64);
//     free(a);

//     void *b = malloc(64);

//     ASSERT(b != NULL, "reuse failed");
//     ASSERT(b == a, "block not reused");

//     free(b);
//     printf("[OK] reuse\n");
//     return 0;
// }

// int test_split(void)
// {
//     TEST("split");

//     void *a = malloc(200);
//     void *b = malloc(64);

//     ASSERT(a != NULL && b != NULL, "split allocation failed");

//     free(a);
//     free(b);
//     printf("[OK] split\n");
//     return 0;
// }

// int test_many_allocs(void)
// {
//     TEST("many allocs");

//     void *ptrs[100];

//     for (int i = 0; i < 100; i++)
//     {
//         ptrs[i] = malloc(64);
//         ASSERT(ptrs[i] != NULL, "allocation failed");
//         free(ptrs[i]);
//     }

//     printf("[OK] many allocs\n");
//     return 0;
// }

// int test_large(void)
// {
//     TEST("large alloc");

//     void *a = malloc(100000);
//     void *b = malloc(200000);

//     ASSERT(a != NULL && b != NULL, "large malloc failed");

//     free(a);
//     free(b);
//     printf("[OK] large alloc\n");
//     return 0;
// }

int shrink_test(void)
{
    TEST("realloc shrink");

    void *a = malloc(200);
    memset(a, 42, 200);

    void *b = realloc(a, 100);
    ASSERT(b != NULL, "realloc failed");
    ASSERT(memcmp(b, a, 100) == 0, "data not preserved on shrink");

    free(b);
    printf("[OK] realloc shrink\n");
    return 0;
}

int grow_in_place(void) {
    TEST("realloc grow in place");

    void *a = malloc(100);
    memset(a, 42, 100);

    void *b = realloc(a, 200);
    ASSERT(b != NULL, "realloc failed");
    ASSERT(memcmp(b, a, 100) == 0, "data not preserved on grow");

    free(b);
    printf("[OK] realloc grow in place\n");
    return 0;
}

int move_case(void) {
    TEST("realloc move");

    void *a = malloc(100);
    memset(a, 42, 100);

    void *old = a;

    a = realloc(a, 200000);
    
    ASSERT(a != NULL, "realloc failed");
    ASSERT(a != old, "expected move, but pointer is the same");

    free(a);
    printf("[OK] realloc move\n");
    return 0;
}

int main(void)
{
    int res = 0;

    // res |= test_basic_malloc();
    // res |= test_free_reuse();
    // res |= test_split();
    // res |= test_many_allocs();
    // res |= test_large();

    res |= shrink_test();
    res |= grow_in_place();
    res |= move_case();

    if (res == 0)
        printf("\nALL TESTS PASSED\n");
    else
        printf("\nSOME TESTS FAILED\n");
    check_leaks();
    return res;
}