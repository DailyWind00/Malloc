#include "ft_malloc.h"

void print_str(const char *str)
{
	while (*str)
		write(1, str++, 1);
}

void print_hex(size_t num)
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

void print_nbr(size_t num)
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

	if (!chunk) {
		print_str("  No allocations in this zone.\n");
		return 0;
	}

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



static void print_byte(unsigned char byte)
{
    const char *hex = "0123456789ABCDEF";

    write(1, &hex[(byte >> 4) & 0xF], 1);
    write(1, &hex[byte & 0xF], 1);
}

static void print_hex_dump(Chunk *chunk)
{
    unsigned char *data;
    size_t i;

    if (chunk->is_free)
        return;

    data = (unsigned char *)chunk + sizeof(Chunk);

    print_str("  Data (first 256 bytes):\n  ");

	size_t dump_size = chunk->size;

	if (dump_size > HEX_DUMP_MAX)
		dump_size = HEX_DUMP_MAX;

    i = 0;
    while (i < dump_size) {
        print_byte(data[i]);
        print_str(" ");

        if ((i + 1) % 16 == 0 && i + 1 < dump_size)
            print_str("\n  ");

        i++;
    }

    print_str("\n\n");
}

static void print_chunk_ex(Chunk *chunk)
{
    print_str("  Chunk   : ");
    print_hex((size_t)chunk);
    print_str("\n");

    print_str("  Data    : ");
    print_hex((size_t)((char *)chunk + sizeof(Chunk)));
    print_str("\n");

    print_str("  Size    : ");
    print_nbr(chunk->size);
    print_str(" bytes\n");

    print_str("  Status  : ");

    if (chunk->is_free)
        print_str("FREE\n");
    else
        print_str("ALLOCATED\n");

    print_str("  Prev    : ");
    print_hex((size_t)chunk->prev);
    print_str("\n");

    print_str("  Next    : ");
    print_hex((size_t)chunk->next);
    print_str("\n");
}

void show_alloc_mem_ex()
{
    size_t total_allocated = 0;
    size_t total_free = 0;

    if (!is_malloc_init()) {
        print_str("Malloc not initialized.\n");
        return;
    }

    print_str("=== Extended Memory Allocations ===\n");

    print_str("\nTINY : ");
    print_hex((size_t)g_zones->tiny);
    print_str("\n");

    Chunk *chunk = g_zones->tiny;

    while (chunk) {
        print_chunk_ex(chunk);
        print_hex_dump(chunk);

        if (chunk->is_free)
            total_free += chunk->size;
        else
            total_allocated += chunk->size;

        chunk = chunk->next;
    }

    print_str("\nSMALL : ");
    print_hex((size_t)g_zones->small);
    print_str("\n");

    chunk = g_zones->small;

    while (chunk) {
        print_chunk_ex(chunk);
        print_hex_dump(chunk);

        if (chunk->is_free)
            total_free += chunk->size;
        else
            total_allocated += chunk->size;

        chunk = chunk->next;
    }

    print_str("\nLARGE : ");
    print_hex((size_t)g_zones->large);
    print_str("\n");

    chunk = g_zones->large;

    while (chunk) {
        print_chunk_ex(chunk);
        print_hex_dump(chunk);

        if (chunk->is_free)
            total_free += chunk->size;
        else
            total_allocated += chunk->size;

        chunk = chunk->next;
    }

    print_str("\nTotal allocated : ");
    print_nbr(total_allocated);
    print_str(" bytes\n");

    print_str("Total free      : ");
    print_nbr(total_free);
    print_str(" bytes\n");

    print_str("==================================\n");
}