#include "malloc.h"

void    free(void *ptr) {
    if (!ptr)
        return ;

    t_block *block = (t_block *)ptr - 1;
    block->free = 1;

    while (block->next && block->next->free)
    {
        block->size += sizeof(t_block) + block->next->size;
        block->next = block->next->next;
    }
}

void    check_leaks(void) {
    t_zone_type types[] = {TINY, SMALL, LARGE};
    size_t      leaked = 0;

    for (int i = 0; i < 3; i++)
    {
        t_zone *zone = *get_zones_list(types[i]);
        while (zone) {
            t_block *block = zone->blocks;
            while (block) {
                if (!block->free)
                    leaked += block->size;
                block = block->next;
            }
            zone = zone->next;
        }
    }
    printf("LEAKS: %zu bytes not freed\n", leaked);
}