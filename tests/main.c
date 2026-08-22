#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void test_malloc(void)
{
    printf("\n=== malloc ===\n");

    char *ptr = malloc(32);

    if (!ptr)
    {
        printf("malloc failed\n");
        return;
    }

    strcpy(ptr, "Hello from my malloc!");

    printf("ptr  = %p\n", (void *)ptr);
    printf("data = \"%s\"\n", ptr);

    free(ptr);

    printf("free() called\n");
}

static void test_realloc_grow(void)
{
    printf("\n=== realloc: grow ===\n");

    char *ptr = malloc(16);

    if (!ptr)
    {
        printf("malloc failed\n");
        return;
    }

    strcpy(ptr, "Hello");

    printf("before realloc:\n");
    printf("ptr  = %p\n", (void *)ptr);
    printf("data = \"%s\"\n", ptr);

    char *new_ptr = realloc(ptr, 64);

    if (!new_ptr)
    {
        printf("realloc failed\n");
        free(ptr);
        return;
    }

    ptr = new_ptr;

    strcat(ptr, " world!");

    printf("after realloc:\n");
    printf("ptr  = %p\n", (void *)ptr);
    printf("data = \"%s\"\n", ptr);

    free(ptr);
}

static void test_realloc_shrink(void)
{
    printf("\n=== realloc: shrink ===\n");

    char *ptr = malloc(128);

    if (!ptr)
    {
        printf("malloc failed\n");
        return;
    }

    strcpy(ptr, "This should survive realloc.");

    printf("before realloc:\n");
    printf("ptr  = %p\n", (void *)ptr);
    printf("data = \"%s\"\n", ptr);

    char *new_ptr = realloc(ptr, 32);

    if (!new_ptr)
    {
        printf("realloc failed\n");
        free(ptr);
        return;
    }

    ptr = new_ptr;

    printf("after realloc:\n");
    printf("ptr  = %p\n", (void *)ptr);
    printf("data = \"%s\"\n", ptr);

    free(ptr);
}

static void test_multiple_allocations(void)
{
    printf("\n=== multiple allocations ===\n");

    void *ptrs[10];

    for (int i = 0; i < 10; i++)
    {
        ptrs[i] = malloc((i + 1) * 16);

        printf("malloc(%d) -> %p\n",
               (i + 1) * 16,
               ptrs[i]);

        if (!ptrs[i])
        {
            printf("malloc failed at index %d\n", i);

            for (int j = 0; j < i; j++)
                free(ptrs[j]);

            return;
        }
    }

    printf("Freeing allocations in reverse order...\n");

    for (int i = 9; i >= 0; i--)
    {
        printf("free(%p)\n", ptrs[i]);
        free(ptrs[i]);
    }
}

static void test_reuse(void)
{
    printf("\n=== memory reuse ===\n");

    void *a = malloc(64);

    printf("malloc(64) -> %p\n", a);

    free(a);

    printf("free(%p)\n", a);

    void *b = malloc(64);

    printf("malloc(64) -> %p\n", b);

    if (a == b)
        printf("Block was reused.\n");
    else
        printf("Block was not reused.\n");

    free(b);
}

static void test_null(void)
{
    printf("\n=== NULL handling ===\n");

    printf("free(NULL)...\n");
    free(NULL);

    printf("free(NULL) completed\n");

    printf("realloc(NULL, 32)...\n");

    void *ptr = realloc(NULL, 32);

    printf("realloc(NULL, 32) -> %p\n", ptr);

    free(ptr);
}

int main(void)
{
    printf("========================================\n");
    printf("       libft_malloc test program\n");
    printf("========================================\n");

    test_malloc();
    test_realloc_grow();
    test_realloc_shrink();
    test_multiple_allocations();
    test_reuse();
    test_null();

    printf("\n========================================\n");
    printf("Tests completed\n");
    printf("========================================\n");

    return 0;
}