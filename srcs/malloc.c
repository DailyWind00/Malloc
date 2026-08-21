#include "ft_malloc.h"

void	free(void *ptr)
{
	if (!is_malloc_init() || !ptr) return;

	pthread_mutex_lock(&g_malloc_mutex);
	
	Chunk *chunk = (Chunk *)((char *)ptr - sizeof(Chunk));

	if (chunk->is_free) {
		pthread_mutex_unlock(&g_malloc_mutex);
		return;
	}

	chunk->is_free = true;
	coalesce_chunk(chunk);

	// If zone is empty, mummap

	pthread_mutex_unlock(&g_malloc_mutex);
}



void	*malloc(size_t size)
{
	if (!is_malloc_init() || size == 0) return NULL;

	// Search for freed block
	// If no : create a new one
	// If yes : try to split it and use it
	return NULL;
}



void	*realloc(void *ptr, size_t size)
{
	if (!is_malloc_init() || size == 0) return NULL;
	if (!ptr) return malloc(size);

	// If size <= current size : shrink block
	// Else if there is enough place after block : expand block
	// Else : malloc > memcpy > free block
	return NULL;
}