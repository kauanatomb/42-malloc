#include "malloc.h"

static void	print_zone_info(t_zone *zones, char *type_name)
{
	t_zone	*zone;
	t_block	*block;

	if (!zones)
		return ;
	ft_putstr_fd(type_name, 1);
	ft_putstr_fd(" : ", 1);
	ft_putstr_fd("0x", 1);
	ft_putstr_fd(ft_itoa((uintptr_t)zones), 1);
	ft_putstr_fd("\n", 1);
	zone = zones;
	while (zone)
	{
		block = zone->blocks;
		while (block)
		{
			if (!block->free)
			{
				ft_putstr_fd("0x", 1);
				ft_putstr_fd(ft_itoa((uintptr_t)get_block_payload(block)), 1);
				ft_putstr_fd(" - 0x", 1);
				ft_putstr_fd(ft_itoa((uintptr_t)get_block_payload(block) + block->size), 1);
				ft_putstr_fd(" : ", 1);
				ft_putnbr_fd(block->size, 1);
				ft_putstr_fd(" bytes\n", 1);
			}
			block = block->next;
		}
		zone = zone->next;
	}
}

void	show_alloc_mem(void)
{
	size_t	total;

	pthread_mutex_lock(&g_malloc_lock);

	total = 0;

	t_zone *tiny = *get_zones_list(TINY);
	t_zone *small = *get_zones_list(SMALL);
	t_zone *large = *get_zones_list(LARGE);

	t_zone *zone;
	t_block *block;

	print_zone_info(tiny, "TINY");
	zone = tiny;
	while (zone)
	{
		block = zone->blocks;
		while (block)
		{
			if (!block->free)
				total += block->size;
			block = block->next;
		}
		zone = zone->next;
	}

	print_zone_info(small, "SMALL");
	zone = small;
	while (zone)
	{
		block = zone->blocks;
		while (block)
		{
			if (!block->free)
				total += block->size;
			block = block->next;
		}
		zone = zone->next;
	}

	print_zone_info(large, "LARGE");
	zone = large;
	while (zone)
	{
		block = zone->blocks;
		while (block)
		{
			if (!block->free)
				total += block->size;
			block = block->next;
		}
		zone = zone->next;
	}

	ft_putstr_fd("Total : ", 1);
	ft_putnbr_fd(total, 1);
	ft_putstr_fd(" bytes\n", 1);

	pthread_mutex_unlock(&g_malloc_lock);
}
