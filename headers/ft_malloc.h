#pragma once

# include <stdlib.h>
# include <stdbool.h>

typedef struct Chunk {
	size_t	size;
	bool	is_free;

	struct Chunk	*prev;
	struct Chunk	*next;
} Chunk;

// static Chunk *g_chunk_start = NULL;

// malloc.c
void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);

// malloc_utils.c
void	coalesce_chunk(Chunk *chunk);