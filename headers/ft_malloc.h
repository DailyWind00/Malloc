#pragma once

#define _GNU_SOURCE

# include <sys/mman.h>
# include <stdbool.h>
# include <pthread.h>
# include <unistd.h>

# define MIN_ALLOC_SIZE sizeof(char)
# define TINY_MAX_SIZE 516
# define SMALL_MAX_SIZE 4096
# define ALIGNMENT 8
# define MAX_ALLOWED_SIZE __SIZE_MAX__ - sizeof(Chunk)

typedef struct Chunk {
	size_t	size;
	bool	is_free;

	struct Chunk	*prev;
	struct Chunk	*next;
} Chunk;

typedef struct Zone {
	Chunk	*tiny;
	Chunk	*small;
	Chunk	*large;

	size_t	page_size;
} Zone;

extern Zone	*g_zones;
extern pthread_mutex_t g_malloc_mutex;

// --- External functions :

// malloc.c
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);
void	free(void *ptr);

// --- Internal functions :

// init.c
bool	is_malloc_init();
bool	can_malloc_exit();
void	init_malloc();
void	exit_malloc();

// malloc_utils.c
size_t	align_size(size_t size);
void	*ft_memcpy(void *dest, const void *src, size_t size);
void	coalesce_chunk(Chunk *chunk);