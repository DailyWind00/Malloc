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

	exit_malloc();
	pthread_mutex_unlock(&g_malloc_mutex);
}



// Search for a free block with enough size inside the given zone.
// Return NULL if no block is found, or zone is NULL
static Chunk *find_free_block(Chunk *zone, size_t size)
{
	while (zone) {
		if (zone->is_free && zone->size >= size)
			return zone;

		zone = zone->next;
	}

	return NULL;
}

// Split in chunk in two if the chunk size is more than the wanted_size
static void try_split_chunk(Chunk *chunk, size_t wanted_size)
{
	if (!chunk || chunk->size <= wanted_size + sizeof(Chunk) + MIN_ALLOC_SIZE) return;

	size_t total_size = chunk->size;
	chunk->size = wanted_size;

	Chunk *new_chunk = (Chunk *)((char *)chunk + sizeof(Chunk) + wanted_size);

	new_chunk->size = total_size - wanted_size - sizeof(Chunk);
	new_chunk->is_free = true;

	new_chunk->next = chunk->next;
	new_chunk->prev = chunk;

	if (chunk->next)
		chunk->next->prev = new_chunk;
	chunk->next = new_chunk;
}

void	*malloc(size_t size)
{
	pthread_mutex_lock(&g_malloc_mutex);
	init_malloc();

	if (!is_malloc_init() || size == 0) {
		pthread_mutex_unlock(&g_malloc_mutex);
		return NULL;
	}

	size = align_size(size);

	// Search for freed block
	// If no : Search in another zone
	// If yes : try to split it and use it

	// Tiny allocation
	if (size < (g_zones->page_size * TINY_MAX_SIZE) - sizeof(Chunk)) {
		Chunk *chunk = find_free_block(g_zones->tiny, size);

		if (chunk) {
			try_split_chunk(chunk, size);
			chunk->is_free = false;

			void *ptr = (char *)chunk + sizeof(Chunk);

			pthread_mutex_unlock(&g_malloc_mutex);
			return ptr;
		}
	}
	// Small allocation
	if (size < (g_zones->page_size * SMALL_MAX_SIZE) - sizeof(Chunk)) {
		Chunk *chunk = find_free_block(g_zones->small, size);

		if (chunk) {
			try_split_chunk(chunk, size);
			chunk->is_free = false;

			void *ptr = (char *)chunk + sizeof(Chunk);

			pthread_mutex_unlock(&g_malloc_mutex);
			return ptr;
		}
	}
	// Large allocation

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