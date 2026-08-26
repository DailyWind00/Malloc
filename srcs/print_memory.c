#include "ft_malloc.h"

static void print_str(const char *str)
{
	while (*str)
		write(1, str++, 1);
}

static void print_hex(size_t num)
{
	char hex[17] = "0123456789ABCDEF";
	char buffer[17];
	int i = 0;

	if (num == 0) {
		write(1, "0", 1);
		return;
	}

	while (num > 0) {
		buffer[i++] = hex[num % 16];
		num /= 16;
	}

	while (--i >= 0)
		write(1, &buffer[i], 1);
}

static void print_nbr(size_t num)
{
	char buffer[21];
	int i = 0;

	if (num == 0) {
		write(1, "0", 1);
		return;
	}

	while (num > 0) {
		buffer[i++] = '0' + (num % 10);
		num /= 10;
	}

	while (--i >= 0)
		write(1, &buffer[i], 1);
}

static size_t print_zone(const char *zone_name, Chunk *zone)
{
	print_str(zone_name);
	print_hex((size_t)zone);
	print_str("\n");

	Chunk *chunk = zone;
	size_t total_size = 0;

    while (chunk)
    {
        if (chunk->is_free)
        {
            print_str("  [FREE]      ");
            print_nbr(chunk->size);
            print_str(" free bytes\n");
        }
        else
        {
            print_str("  [ALLOCATED] ");

            print_hex((size_t)((char *)chunk + sizeof(Chunk)));
            print_str(" - ");
            print_hex((size_t)((char *)chunk + sizeof(Chunk) + chunk->size));
            print_str(" : ");

            print_nbr(chunk->size);
            print_str(" bytes\n");

            total_size += chunk->size;
        }

        chunk = chunk->next;
    }

    return total_size;
}

void	show_alloc_mem()
{
	pthread_mutex_lock(&g_malloc_mutex);

	print_str("=== Memory Allocations ===\n");

	if (!g_zones) {
		print_str("Malloc not initialized.\n");
		print_str("==========================\n");
		pthread_mutex_unlock(&g_malloc_mutex);
		return;
	}
	size_t total_size = 0;

	total_size += print_zone("TINY : ",  g_zones->tiny);
	total_size += print_zone("SMALL : ", g_zones->small);
	total_size += print_zone("LARGE : ", g_zones->large);

	print_str("Total : ");
	print_nbr(total_size);
	print_str(" bytes\n");

	print_str("==========================\n");

	pthread_mutex_unlock(&g_malloc_mutex);
}