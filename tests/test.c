#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <unistd.h>
#include "ft_malloc.h"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

static int test_malloc(void)
{
    printf("=== malloc ===\n");

	long psize = sysconf(_SC_PAGE_SIZE);

    char *tiny = malloc(32 * psize);

    if (tiny == NULL)
    {
        printf("[FAIL] tiny malloc returned NULL\n");
        return 1;
    }

    strcpy(tiny, "Hello malloc!");

    if (strcmp(tiny, "Hello malloc!") != 0)
    {
        printf("[FAIL] memory cannot be written/read\n");
        free(tiny);
        return 1;
    }
    printf("[OK] tiny malloc: %p -> \"%s\"\n", (void *)tiny, tiny);

	char *small = malloc(700 * psize);
	char *large = malloc(5000 * psize);

	if (small == NULL)
	{
		printf("[FAIL] small malloc returned NULL\n");
		return 1;
	}
	printf("[OK] small malloc: %p\n", (void *)small);

	if (large == NULL)
	{
		printf("[FAIL] large malloc returned NULL\n");
		return 1;
	}
	printf("[OK] large malloc: %p\n", (void *)large);

    free(tiny);
	free(small);
	free(large);

    printf("[OK] free\n");

    return 0;
}

static int test_realloc(void)
{
    printf("\n=== realloc ===\n");

    char *ptr = malloc(16);

    if (ptr == NULL)
    {
        printf("[FAIL] initial malloc\n");
        return 1;
    }

    strcpy(ptr, "Hello");

    char *new_ptr = realloc(ptr, 64);

    if (new_ptr == NULL)
    {
        printf("[FAIL] realloc returned NULL\n");
        free(ptr);
        return 1;
    }

    ptr = new_ptr;

    if (strcmp(ptr, "Hello") != 0)
    {
        printf("[FAIL] realloc did not preserve data\n");
        free(ptr);
        return 1;
    }

    printf("[OK] realloc: %p -> \"%s\"\n", (void *)ptr, ptr);

    free(ptr);

    return 0;
}

static int test_multiple_allocations(void)
{
    printf("\n=== multiple allocations ===\n");

    void *ptrs[10];

    for (int i = 0; i < 10; ++i)
    {
        ptrs[i] = malloc((i + 1) * 16);

        if (ptrs[i] == NULL)
        {
            printf("[FAIL] malloc #%d\n", i);

            for (int j = 0; j < i; ++j)
                free(ptrs[j]);

            return 1;
        }

        printf("[OK] malloc #%d: %p\n", i, ptrs[i]);
    }

    for (int i = 0; i < 10; ++i)
        free(ptrs[i]);

    printf("[OK] all allocations freed\n");

    return 0;
}

static int test_null(void)
{
    printf("\n=== NULL ===\n");

    free(NULL);

    printf("[OK] free(NULL)\n");

    void *ptr = realloc(NULL, 32);

    if (ptr == NULL)
    {
        printf("[FAIL] realloc(NULL, 32)\n");
        return 1;
    }

    printf("[OK] realloc(NULL, 32): %p\n", ptr);

    free(ptr);

    return 0;
}

int main(void)
{
    int failures = 0;

    failures += test_malloc();
    failures += test_realloc();
    failures += test_multiple_allocations();
    failures += test_null();

    printf("\n============================\n");

    if (failures == 0) {
        printf("ALL TESTS PASSED\n");
	} else {
		printf("%d TEST(S) FAILED\n", failures);
	}

    return failures != 0;
}