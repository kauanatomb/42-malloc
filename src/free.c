#include "malloc.h"

void    free(void *ptr) {
    if (!ptr)
        return ;

    pthread_mutex_lock(&g_malloc_lock);

    if (!is_valid_ptr(ptr)) {
        pthread_mutex_unlock(&g_malloc_lock);
        return;
    }

    t_block *block = (t_block *)ptr - 1;
    block->free = 1;

    if (block->prev && block->prev->free) {
        t_block *prev = block->prev;
        prev->size += sizeof(t_block) + block->size;
        prev->next = block->next;
        if (block->next)
            block->next->prev = prev;
        block = prev;
    }

    if (block->next && block->next->free) {
        t_block *next = block->next;
        block->size += sizeof(t_block) + next->size;
        block->next = next->next;
        if (block->next)
            block->next->prev = block;
    }

    if (!block->prev && !block->next)
        cleanup_zone_if_empty(block_to_zone(block));

    pthread_mutex_unlock(&g_malloc_lock);
}

void    check_leaks(void) {
    t_zone_type types[] = {TINY, SMALL, LARGE};
    size_t      leaked = 0;

    pthread_mutex_lock(&g_malloc_lock);

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

    pthread_mutex_unlock(&g_malloc_lock);

    ft_putstr_fd("Total : ", 1);
    ft_putnbr_fd(leaked, 1);
    ft_putstr_fd(" bytes\n", 1);
}