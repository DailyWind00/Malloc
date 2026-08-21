#include "ft_malloc.h"

Zone	*g_zones = NULL;
pthread_mutex_t g_malloc_mutex = PTHREAD_MUTEX_INITIALIZER;

static Chunk *get_zone(size_t zsize)
{
	void *alloc = mmap(NULL, zsize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

	if (alloc == MAP_FAILED)
		return NULL;

	Chunk *zone = alloc;

	zone->is_free = true;
	zone->size = zsize - sizeof(Chunk);
	zone->next = NULL;
	zone->prev = NULL;

	return zone;
}

// Create the malloc context to allow the use of the malloc/realloc/free functions.
int	init_malloc()
{
	size_t psize = sysconf(_SC_PAGE_SIZE);

	g_zones->tiny = get_zone(psize * TINY_MAX_SIZE);
	g_zones->small = get_zone(psize * SMALL_MAX_SIZE);
	g_zones->large = NULL;

	if (!g_zones->tiny || !g_zones->small)
		return -1;
	return 0;
}



// Destroy the malloc context, which block the use of the malloc/realloc/free functions.
int	exit_malloc()
{


}