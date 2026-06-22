#include "malloc.h"

int	is_zone_empty(t_zone *zone)
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

t_zone	*block_to_zone(t_block *block)
{
	return ((t_zone *)((char *)block - sizeof(t_zone)));
}

void	cleanup_zone_if_empty(t_zone *zone)
{
	t_zone	**head;
	t_zone	*cur;

	if (!zone || !is_zone_empty(zone))
		return;
	head = get_zones_list(zone->type);
	if (*head == zone)
		*head = zone->next;
	else
	{
		cur = *head;
		while (cur && cur->next != zone)
			cur = cur->next;
		if (cur)
			cur->next = zone->next;
	}
	munmap(zone, zone->size);
}
