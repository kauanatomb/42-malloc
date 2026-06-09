#include "malloc.h"

static int	is_zone_empty(t_zone *zone)
{
	t_block *block;

	block = zone->blocks;
	while (block)
	{
		if (!block->free)
			return (0);
		block = block->next;
	}
	return (1);
}

void	cleanup_empty_zones(void)
{
	t_zone_type	types[] = {TINY, SMALL, LARGE};
	int			i;
	t_zone		**zones_list;
	t_zone		*zone;
	t_zone		*prev;

	i = 0;
	while (i < 3)
	{
		zones_list = get_zones_list(types[i]);
		zone = *zones_list;
		prev = NULL;

		while (zone)
		{
			if (is_zone_empty(zone))
			{
				if (prev)
					prev->next = zone->next;
				else
					*zones_list = zone->next;

				munmap(zone, zone->size);
				zone = prev ? prev->next : *zones_list;
			}
			else
			{
				prev = zone;
				zone = zone->next;
			}
		}
		i++;
	}
}
