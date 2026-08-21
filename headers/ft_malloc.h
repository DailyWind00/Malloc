#pragma once

# include <stdlib.h>
# include <stdbool.h>
# include <pthread.h>

typedef struct Chunk {
	size_t	size;
	bool	is_free;

	struct Chunk	*prev;
	struct Chunk	*next;
} Chunk;

static Chunk *g_chunk_start = NULL;
static pthread_mutex_t g_malloc_mutex = PTHREAD_MUTEX_INITIALIZER;

// malloc.c
void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);

// malloc_utils.c
size_t	align_size(size_t size, size_t alignment);
void	coalesce_chunk(Chunk *chunk);