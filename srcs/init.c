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
	if (is_malloc_init()) return 0;

	size_t psize = sysconf(_SC_PAGE_SIZE);

	g_zones = mmap(NULL, sizeof(Zone), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (g_zones == MAP_FAILED)
		return -1;

	g_zones->tiny = get_zone(psize * TINY_MAX_SIZE);
	g_zones->small = get_zone(psize * SMALL_MAX_SIZE);
	g_zones->large = NULL;

	if (!g_zones->tiny || !g_zones->small)
		return -1;
	return 0;
}



// Destroy the malloc context, which block the use of the malloc/realloc/free functions.
void	exit_malloc()
{
	if (!can_malloc_exit()) return;

	if (g_zones->tiny)	munmap(g_zones->tiny,  g_zones->tiny->size  + sizeof(Chunk));
	if (g_zones->small)	munmap(g_zones->small, g_zones->small->size + sizeof(Chunk));
	if (g_zones->large)	munmap(g_zones->large, g_zones->large->size + sizeof(Chunk));

	munmap(g_zones, sizeof(Zone));
}