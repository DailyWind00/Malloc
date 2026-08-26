#include "ft_malloc.h"

Zone	*g_zones = NULL;
pthread_mutex_t g_malloc_mutex = PTHREAD_MUTEX_INITIALIZER;

// Check if the malloc context have been initialized with init_malloc().
bool	is_malloc_init() {
	return g_zones;
}

// Check if the malloc context can be safely destroyed with exit_malloc().
bool	can_malloc_exit() {
	if (!is_malloc_init()) return false;

	bool can_exit = true;

	while (can_exit && g_zones->tiny) {
		can_exit = g_zones->tiny->is_free;
		g_zones->tiny = g_zones->tiny->next;
	}
	
	while (can_exit && g_zones->small) {
		can_exit = g_zones->small->is_free;
		g_zones->small = g_zones->small->next;
	}
	
	return can_exit;
}



static Chunk *get_zone(size_t zsize)
{
	void *alloc = mmap(NULL, zsize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

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
void	init_malloc()
{
	if (is_malloc_init()) return;

	long psize = sysconf(_SC_PAGE_SIZE);
	if (psize == -1) return;

	g_zones = mmap(NULL, sizeof(Zone), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (g_zones == MAP_FAILED) {
		g_zones = NULL;
		return;
	}

	g_zones->tiny = get_zone(psize * TINY_MAX_SIZE);
	if (g_zones->tiny == NULL) {
		munmap(g_zones, sizeof(Zone));
		g_zones = NULL;
		return;
	}

	g_zones->small = get_zone(psize * SMALL_MAX_SIZE);
	if (g_zones->small == NULL) {
		munmap(g_zones->tiny, psize * TINY_MAX_SIZE);
		munmap(g_zones, sizeof(Zone));
		g_zones = NULL;
		return;
	}

	g_zones->large = NULL;
	g_zones->page_size = psize;
}



// Destroy the malloc context, which block the use of the malloc/realloc/free functions.
void	exit_malloc()
{
	if (!can_malloc_exit()) return;

	long psize = sysconf(_SC_PAGE_SIZE);
	if (psize == -1) return;

	if (g_zones->tiny)	munmap(g_zones->tiny,  psize * TINY_MAX_SIZE);
	if (g_zones->small)	munmap(g_zones->small, psize * SMALL_MAX_SIZE);

	while (g_zones->large) {
		Chunk *next = g_zones->large->next;
		munmap(g_zones->large, g_zones->large->size + sizeof(Chunk));
		g_zones->large = next;
	}

	munmap(g_zones, sizeof(Zone));
	g_zones = NULL;
}