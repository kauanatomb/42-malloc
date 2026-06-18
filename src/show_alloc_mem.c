#include "malloc.h"

static void	write_str(const char *s) {
	ft_putstr_fd((char *)s, 1);
}

static void	write_hex(uintptr_t n)
{
	char		buf[20];
	int			i;
	uintptr_t	num;

	if (n == 0)
	{
		ft_putstr_fd("0x0", 1);
		return ;
	}

	num = n;
	i = 0;
	while (num > 0)
	{
		buf[i] = "0123456789abcdef"[num % 16];
		num /= 16;
		i++;
	}

	ft_putstr_fd("0x", 1);
	while (i > 0)
	{
		i--;
		ft_putchar_fd(buf[i], 1);
	}
}

static void	write_number(size_t n)
{
	char	buf[20];
	int		i;
	size_t	num;

	if (n == 0)
	{
		ft_putchar_fd('0', 1);
		return ;
	}

	num = n;
	i = 0;
	while (num > 0)
	{
		buf[i] = '0' + (num % 10);
		num /= 10;
		i++;
	}

	while (i > 0)
	{
		i--;
		ft_putchar_fd(buf[i], 1);
	}
}

static size_t print_block_info(t_block *block)
{
    void *payload = get_block_payload(block);
    write_hex((uintptr_t)payload);
    write_str(" - ");
    write_hex((uintptr_t)payload + block->size);
    write_str(" : ");
    write_number(block->size);
    write_str(" bytes\n");
    return block->size;
}

static size_t print_zone_blocks(t_zone *zone)
{
    size_t total = 0;
    t_block *block = zone->blocks;
    while (block) {
        if (!block->free)
            total += print_block_info(block);
        block = block->next;
    }
    return total;
}

static size_t print_zones_of_type(t_zone_type type, const char *label, int per_zone_header)
{
    t_zone *zone = *get_zones_list(type);
    if (!zone)
        return 0;

    size_t total = 0;

    if (!per_zone_header) {
        write_str(label);
        write_str(" : ");
        write_hex((uintptr_t)zone);
        write_str("\n");
    }

    while (zone) {
        if (per_zone_header) {
            write_str(label);
            write_str(" : ");
            write_hex((uintptr_t)zone);
            write_str("\n");
        }
        total += print_zone_blocks(zone);
        zone = zone->next;
    }
    return total;
}

void show_alloc_mem(void)
{
    pthread_mutex_lock(&g_malloc_lock);

    size_t total = print_zones_of_type(TINY,  "TINY",  0);
    total += print_zones_of_type(SMALL, "SMALL", 0);
    total += print_zones_of_type(LARGE, "LARGE", 1);

    write_str("Total : ");
    write_number(total);
    write_str(" bytes\n");

    pthread_mutex_unlock(&g_malloc_lock);
}
