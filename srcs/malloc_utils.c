#include "ft_malloc.h"

// Round the given size to the nearest alignment multiple greater than size to avoid misalignment.
size_t	align_size(size_t size) {
    if (size > SIZE_MAX - (ALIGNMENT - 1)) {
		return 0;
	}
	return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

void	*ft_memcpy(void *dest, const void *src, size_t size)
{
	if (dest == NULL || src == NULL) return (NULL);

	size_t i = 0;

	while (i < size) {
		((char *)dest)[i] = ((const char *)src)[i];
		i++;
	}

	return (dest);
}



// Check if two chunks are adjacent in memory
static bool	are_chunks_adjacent(Chunk *first, Chunk *second) {
	if (!first || !second) return false;
	return (char *)first + sizeof(Chunk) + first->size == (char *)second;
}

// Coalesce adjacent free chunks into a single larger chunk
void	coalesce_chunk(Chunk *chunk)
{
	if (!chunk || !chunk->is_free) return;

	Chunk *first = chunk;
	Chunk *last = chunk;

    while (first->prev && first->prev->is_free && are_chunks_adjacent(first->prev, first)) {
        first = first->prev;
	}
    while (last->next && last->next->is_free && are_chunks_adjacent(last, last->next)) {
        last = last->next;
	}
	if (first == last) return;

	size_t total_size = 0;
	Chunk *current = first;

	while (current != last->next) {
		total_size += current->size + sizeof(Chunk);
		current = current->next;
	}

	first->size = total_size - sizeof(Chunk);
	first->next = last->next;
	if (last->next)
		last->next->prev = first;
}