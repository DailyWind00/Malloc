#include "ft_malloc.h"

void	free(void *ptr)
{
	if (!ptr)	return;


}



void	*malloc(size_t size)
{
	if (size == 0)	return NULL;

	// Search for freed block
	// If no : create a new one
	// If yes : try to split it and use it
	return NULL;
}



void	*realloc(void *ptr, size_t size)
{
	if (!ptr)		return malloc(size);
	if (size == 0)	return NULL;

	// If size <= current size : shrink block
	// Else if there is enough place after block : expand block
	// Else : malloc > memcpy > free block
	return NULL;
}