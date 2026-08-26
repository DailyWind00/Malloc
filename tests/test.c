#include "ft_malloc.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

static int g_tests = 0;
static int g_failed = 0;

static void test_start(const char *name)
{
    printf("\n[TEST] %s\n", name);
    g_tests++;
}

static void test_fail(const char *msg)
{
    printf("  FAIL: %s\n", msg);
    g_failed++;
}

static void test_pass(void)
{
    printf("  PASS\n");
}

static int is_aligned(void *ptr)
{
    return ((uintptr_t)ptr % ALIGNMENT) == 0;
}

/*
 * ------------------------------------------------------------
 * malloc()
 * ------------------------------------------------------------
 */

static void test_malloc_basic(void)
{
    test_start("malloc basic allocation");

    void *ptr = malloc(42);

    if (!ptr)
    {
        test_fail("malloc(42) returned NULL");
        return;
    }

    memset(ptr, 0xAA, 42);

    if (!is_aligned(ptr))
    {
        test_fail("returned pointer is not aligned");
        free(ptr);
        return;
    }

    free(ptr);
    test_pass();
}

static void test_malloc_multiple(void)
{
    test_start("multiple malloc allocations");

    void *a = malloc(16);
    void *b = malloc(32);
    void *c = malloc(64);
    void *d = malloc(128);

    if (!a || !b || !c || !d)
    {
        test_fail("one of the allocations returned NULL");
        free(a);
        free(b);
        free(c);
        free(d);
        return;
    }

    if (a == b || a == c || a == d ||
        b == c || b == d ||
        c == d)
    {
        test_fail("two allocations returned the same address");
        free(a);
        free(b);
        free(c);
        free(d);
        return;
    }

    memset(a, 0x11, 16);
    memset(b, 0x22, 32);
    memset(c, 0x33, 64);
    memset(d, 0x44, 128);

    free(a);
    free(b);
    free(c);
    free(d);

    test_pass();
}

static void test_malloc_zero(void)
{
    test_start("malloc(0)");

    void *ptr = malloc(0);

    /*
     * My implementation currently returns NULL.
     */
    if (ptr != NULL)
    {
        test_fail("malloc(0) should return NULL for this implementation");
        free(ptr);
        return;
    }

    test_pass();
}

static void test_malloc_alignment(void)
{
    test_start("malloc alignment");

    void *ptrs[20];
    size_t sizes[20] = {
        1, 2, 3, 7, 8,
        9, 15, 16, 17, 31,
        32, 33, 63, 64, 65,
        127, 128, 129, 255, 256
    };

    for (int i = 0; i < 20; i++)
    {
        ptrs[i] = malloc(sizes[i]);

        if (!ptrs[i])
        {
            test_fail("malloc returned NULL");
            for (int j = 0; j < i; j++)
                free(ptrs[j]);
            return;
        }

        if (!is_aligned(ptrs[i]))
        {
            test_fail("allocation is not correctly aligned");

            for (int j = 0; j <= i; j++)
                free(ptrs[j]);

            return;
        }
    }

    for (int i = 0; i < 20; i++)
        free(ptrs[i]);

    test_pass();
}

/*
 * ------------------------------------------------------------
 * TINY / SMALL / LARGE
 * ------------------------------------------------------------
 */

static void test_size_classes(void)
{
    test_start("TINY / SMALL / LARGE allocations");

	size_t psize = sysconf(_SC_PAGE_SIZE);

    void *tiny = malloc(32 * psize);
    void *small = malloc(1024);
    void *large = malloc(1024 * 1024);

    if (!tiny || !small || !large)
    {
        test_fail("one of the size classes returned NULL");
        free(tiny);
        free(small);
        free(large);
        return;
    }

    memset(tiny, 0x11, 32);
    memset(small, 0x22, 1024);
    memset(large, 0x33, 1024 * 1024);

    free(tiny);
    free(small);
    free(large);

    test_pass();
}

/*
 * ------------------------------------------------------------
 * Reuse / splitting
 * ------------------------------------------------------------
 */

static void test_reuse_freed_block(void)
{
    test_start("reuse freed block");

    void *a = malloc(128);

    if (!a)
    {
        test_fail("initial malloc failed");
        return;
    }

    free(a);

    void *b = malloc(128);

    if (!b)
    {
        test_fail("second malloc failed");
        return;
    }

    /*
     * With your current allocator design, this should normally
     * reuse the freed chunk.
     */
    if (b != a)
    {
        printf("  WARNING: allocator did not reuse the same address\n");
    }

    free(b);
    test_pass();
}

static void test_split(void)
{
    test_start("chunk splitting");

    void *large = malloc(512);

    if (!large)
    {
        test_fail("large allocation failed");
        return;
    }

    free(large);

    void *a = malloc(64);
    void *b = malloc(64);

    if (!a || !b)
    {
        test_fail("allocation after split failed");
        free(a);
        free(b);
        return;
    }

    if (a == b)
    {
        test_fail("two allocations overlap");
        free(a);
        free(b);
        return;
    }

    free(a);
    free(b);

    test_pass();
}

static void test_coalescing(void)
{
    test_start("free and coalescing");

    void *a = malloc(64);
    void *b = malloc(64);
    void *c = malloc(64);

    if (!a || !b || !c)
    {
        test_fail("initial allocations failed");
        free(a);
        free(b);
        free(c);
        return;
    }

    free(a);
    free(b);

    /*
     * This allocation is large enough that it may require
     * the two adjacent free blocks to have been coalesced.
     *
     * Adjust the size according to your Chunk header.
     */
    void *merged = malloc(128);

    if (!merged)
    {
        test_fail("allocation after coalescing failed");
        free(c);
        return;
    }

    free(merged);
    free(c);

    test_pass();
}

/*
 * ------------------------------------------------------------
 * free()
 * ------------------------------------------------------------
 */

static void test_free_null(void)
{
    test_start("free(NULL)");

    free(NULL);

    test_pass();
}

static void test_free_multiple(void)
{
    test_start("free multiple allocations");

    void *ptrs[100];

    for (int i = 0; i < 100; i++)
    {
        ptrs[i] = malloc(i + 1);

        if (!ptrs[i])
        {
            test_fail("allocation failed");
            for (int j = 0; j < i; j++)
                free(ptrs[j]);
            return;
        }
    }

    for (int i = 0; i < 100; i++)
        free(ptrs[i]);

    test_pass();
}

/*
 * ------------------------------------------------------------
 * realloc()
 * ------------------------------------------------------------
 */

static void test_realloc_grow(void)
{
    test_start("realloc growing allocation");

    unsigned char *ptr = malloc(32);

    if (!ptr)
    {
        test_fail("malloc failed");
        return;
    }

    for (int i = 0; i < 32; i++)
        ptr[i] = (unsigned char)i;

    unsigned char *new_ptr = realloc(ptr, 128);

    if (!new_ptr)
    {
        test_fail("realloc failed");
        free(ptr);
        return;
    }

    for (int i = 0; i < 32; i++)
    {
        if (new_ptr[i] != (unsigned char)i)
        {
            test_fail("old data was not preserved");
            free(new_ptr);
            return;
        }
    }

    free(new_ptr);
    test_pass();
}

static void test_realloc_shrink(void)
{
    test_start("realloc shrinking allocation");

    unsigned char *ptr = malloc(256);

    if (!ptr)
    {
        test_fail("malloc failed");
        return;
    }

    for (int i = 0; i < 256; i++)
        ptr[i] = (unsigned char)i;

    unsigned char *new_ptr = realloc(ptr, 64);

    if (!new_ptr)
    {
        test_fail("realloc failed");
        free(ptr);
        return;
    }

    for (int i = 0; i < 64; i++)
    {
        if (new_ptr[i] != (unsigned char)i)
        {
            test_fail("data was corrupted after shrinking");
            free(new_ptr);
            return;
        }
    }

    free(new_ptr);
    test_pass();
}

static void test_realloc_same_size(void)
{
    test_start("realloc same size");

    void *ptr = malloc(128);

    if (!ptr)
    {
        test_fail("malloc failed");
        return;
    }

    void *new_ptr = realloc(ptr, 128);

    if (new_ptr != ptr)
    {
        test_fail("realloc changed pointer unnecessarily");
        free(new_ptr);
        return;
    }

    free(new_ptr);
    test_pass();
}

static void test_realloc_null(void)
{
    test_start("realloc(NULL, size)");

    void *ptr = realloc(NULL, 128);

    if (!ptr)
    {
        test_fail("realloc(NULL, 128) returned NULL");
        return;
    }

    free(ptr);
    test_pass();
}

/*
 * ------------------------------------------------------------
 * Overflow / huge allocation
 * ------------------------------------------------------------
 */

static void test_huge_allocation(void)
{
    test_start("huge allocation / overflow protection");

    size_t huge = SIZE_MAX;

    void *ptr = malloc(huge);

    if (ptr != NULL)
    {
        test_fail("malloc(SIZE_MAX) unexpectedly succeeded");
        free(ptr);
        return;
    }

    test_pass();
}

static void test_realloc_huge(void)
{
    test_start("realloc huge size");

    void *ptr = malloc(64);

    if (!ptr)
    {
        test_fail("initial malloc failed");
        return;
    }

	size_t huge = SIZE_MAX;

    void *new_ptr = realloc(ptr, huge);

    if (new_ptr != NULL)
    {
        test_fail("realloc(SIZE_MAX) unexpectedly succeeded");
        free(new_ptr);
        return;
    }

    /*
     * realloc failure must preserve the original allocation.
     */
    memset(ptr, 0xAA, 64);

    free(ptr);

    test_pass();
}

/*
 * ------------------------------------------------------------
 * show_alloc_mem()
 * ------------------------------------------------------------
 */

static void test_show_alloc_mem(void)
{
    test_start("show_alloc_mem");

    void *a = malloc(42);
    void *b = malloc(84);
    void *c = malloc(3725);
	void *d = malloc(48847);

    if (!a || !b || !c || !d)
    {
        test_fail("allocation failed");
        free(a);
        free(b);
        free(c);
		free(d);
        return;
    }

    show_alloc_mem();
    free(a);
	show_alloc_mem();
    free(b);
	show_alloc_mem();
    free(c);
	show_alloc_mem();
	free(d);
	show_alloc_mem();

    test_pass();
}

/*
 * ------------------------------------------------------------
 * Stress test
 * ------------------------------------------------------------
 */

static void test_stress(void)
{
    test_start("allocation stress test");

    enum { COUNT = 1000 };

    void *ptrs[COUNT];

    for (int i = 0; i < COUNT; i++)
    {
        size_t size = (i % 512) + 1;

        ptrs[i] = malloc(size);

        if (!ptrs[i])
        {
            test_fail("stress allocation failed");

            for (int j = 0; j < i; j++)
                free(ptrs[j]);

            return;
        }

        memset(ptrs[i], i & 0xFF, size);
    }

    /*
     * Free in a non-sequential order.
     */
    for (int i = 0; i < COUNT; i += 2)
        free(ptrs[i]);

    for (int i = 1; i < COUNT; i += 2)
        free(ptrs[i]);

    test_pass();
}

/*
 * ------------------------------------------------------------
 * Main
 * ------------------------------------------------------------
 */

int main(void)
{
    printf("========================================\n");
    printf("        ft_malloc test suite\n");
    printf("========================================\n");

    test_malloc_basic();
    test_malloc_multiple();
    test_malloc_zero();
    test_malloc_alignment();

    test_size_classes();

    test_reuse_freed_block();
    test_split();
    test_coalescing();

    test_free_null();
    test_free_multiple();

    test_realloc_grow();
    test_realloc_shrink();
    test_realloc_same_size();
    test_realloc_null();

    test_huge_allocation();
    test_realloc_huge();

    test_show_alloc_mem();

    test_stress();

    printf("\n========================================\n");
    printf("Tests:  %d\n", g_tests);
    printf("Failed: %d\n", g_failed);
    printf("========================================\n");

    return g_failed != 0;
}