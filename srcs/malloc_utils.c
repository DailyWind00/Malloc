#include "ft_malloc.h"

// Round the given size to the nearest alignment multiple greater than size to avoid misalignment.
size_t	align_size(size_t size, size_t alignment) {
	return (size + alignment - 1) & ~(alignment - 1);
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

    while (first->prev && first->prev->is_free && are_chunks_adjacent(first->prev, first))
        first = first->prev;

    while (last->next && last->next->is_free && are_chunks_adjacent(last, last->next))
        last = last->next;

	if (first == last)
        return;

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