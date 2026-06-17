#include "malloc.h"
#include <unistd.h>

static void	write_str(const char *s)
{
	size_t len = 0;
	while (s[len])
		len++;
	write(1, s, len);
}

static void	write_hex(uintptr_t n)
{
	char		buf[20];
	int			i;
	uintptr_t	num;

	if (n == 0)
	{
		write(1, "0x0", 3);
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

	write(1, "0x", 2);
	while (i > 0)
	{
		i--;
		write(1, &buf[i], 1);
	}
}

static void	write_number(size_t n)
{
	char	buf[20];
	int		i;
	size_t	num;

	if (n == 0)
	{
		write(1, "0", 1);
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
		write(1, &buf[i], 1);
	}
}

void	show_alloc_mem(void)
{
	t_zone	*zone;
	t_block	*block;
	size_t	total;

	pthread_mutex_lock(&g_malloc_lock);

	total = 0;

	zone = *get_zones_list(TINY);
	if (zone)
	{
		write_str("TINY : ");
		write_hex((uintptr_t)zone);
		write_str("\n");
		while (zone)
		{
			block = zone->blocks;
			while (block)
			{
				if (!block->free)
				{
					write_hex((uintptr_t)get_block_payload(block));
					write_str(" - ");
					write_hex((uintptr_t)get_block_payload(block) + block->size);
					write_str(" : ");
					write_number(block->size);
					write_str(" bytes\n");
					total += block->size;
				}
				block = block->next;
			}
			zone = zone->next;
		}
	}

	zone = *get_zones_list(SMALL);
	if (zone)
	{
		write_str("SMALL : ");
		write_hex((uintptr_t)zone);
		write_str("\n");
		while (zone)
		{
			block = zone->blocks;
			while (block)
			{
				if (!block->free)
				{
					write_hex((uintptr_t)get_block_payload(block));
					write_str(" - ");
					write_hex((uintptr_t)get_block_payload(block) + block->size);
					write_str(" : ");
					write_number(block->size);
					write_str(" bytes\n");
					total += block->size;
				}
				block = block->next;
			}
			zone = zone->next;
		}
	}

	zone = *get_zones_list(LARGE);
	if (zone)
	{
		write_str("LARGE : ");
		write_hex((uintptr_t)zone);
		write_str("\n");
		while (zone)
		{
			block = zone->blocks;
			while (block)
			{
				if (!block->free)
				{
					write_hex((uintptr_t)get_block_payload(block));
					write_str(" - ");
					write_hex((uintptr_t)get_block_payload(block) + block->size);
					write_str(" : ");
					write_number(block->size);
					write_str(" bytes\n");
					total += block->size;
				}
				block = block->next;
			}
			zone = zone->next;
		}
	}

	write_str("Total : ");
	write_number(total);
	write_str(" bytes\n");

	pthread_mutex_unlock(&g_malloc_lock);
}
