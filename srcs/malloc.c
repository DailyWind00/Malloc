#include "ft_malloc.h"

void	free(void *ptr)
{
	pthread_mutex_lock(&g_malloc_mutex);

	if (!is_malloc_init() || !ptr) return;
	
	Chunk *chunk = (Chunk *)((char *)ptr - sizeof(Chunk));

	if (chunk->is_free) {
		pthread_mutex_unlock(&g_malloc_mutex);
		return;
	}

	chunk->is_free = true;
	coalesce_chunk(chunk);

	// If zone is empty, mummap

	exit_malloc();
	pthread_mutex_unlock(&g_malloc_mutex);
}



void	*malloc(size_t size)
{
	pthread_mutex_lock(&g_malloc_mutex);
	init_malloc();

	if (!is_malloc_init() || size == 0) return NULL;

	// Search for freed block
	// If no : create a new one
	// If yes : try to split it and use it

	pthread_mutex_unlock(&g_malloc_mutex);

	return NULL;
}



void	*realloc(void *ptr, size_t size)
{
	pthread_mutex_lock(&g_malloc_mutex);
	init_malloc();

	if (!is_malloc_init() || size == 0) return NULL;
	if (!ptr) return malloc(size);

	// If size <= current size : shrink block
	// Else if there is enough place after block : expand block
	// Else : malloc > memcpy > free block

	pthread_mutex_unlock(&g_malloc_mutex);

	return NULL;
}