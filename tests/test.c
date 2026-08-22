#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

static int test_malloc(void)
{
    printf("=== malloc ===\n");

    char *ptr = malloc(32);

    if (ptr == NULL)
    {
        printf("[FAIL] malloc returned NULL\n");
        return 1;
    }

    strcpy(ptr, "Hello malloc!");

    if (strcmp(ptr, "Hello malloc!") != 0)
    {
        printf("[FAIL] memory cannot be written/read\n");
        free(ptr);
        return 1;
    }

    printf("[OK] malloc: %p -> \"%s\"\n", (void *)ptr, ptr);

    free(ptr);

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

static int check_malloc_library(void)
{
    Dl_info info;

    if (dladdr((void *)malloc, &info) == 0)
    {
        fprintf(stderr, "Could not determine malloc implementation.\n");
        return 0;
    }

    if (info.dli_fname == NULL)
    {
        fprintf(stderr, "Could not determine malloc library.\n");
        return 0;
    }

    if (strstr(info.dli_fname, "libft_malloc") == NULL)
    {
        fprintf(stderr,
            RED "Error: your libft_malloc is not loaded.\n" RESET
            "Run: "
			GREEN "LD_PRELOAD=./libft_malloc.so ./test_malloc" RESET "\n");

        fprintf(stderr,
            "Current malloc: %s\n",
            info.dli_fname);

        return 0;
    }

	printf(GREEN "> libft_malloc detected.\n" RESET
		"Current malloc: %s\n\n", info.dli_fname);

    return 1;
}

int main(void)
{
	if (!check_malloc_library())
		return 1;

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