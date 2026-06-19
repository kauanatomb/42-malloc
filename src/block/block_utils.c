#include "malloc.h"

void *get_block_payload(t_block *block) {
    return (void *)(block + 1);
}

void    split_block(t_block *block, size_t size) {
    if (block->size < size + sizeof(t_block) + ALIGNMENT)
        return;

    size_t remaining = block->size - size - sizeof(t_block);
    void *payload = block + 1;
    t_block *new_block = (t_block *)((char *)payload + size);
    t_block *old_next = block->next;

    new_block->size = remaining;
    new_block->free = 1;
    new_block->next = old_next;
    new_block->prev = block;

    if (old_next)
        old_next->prev = new_block;

    block->size = size;
    block->next = new_block;
}

t_block *find_free_block(t_zone *zone, size_t size) {
    t_zone *cur = zone;
    while (cur) {
        t_block *cur_b = cur->blocks;
        while (cur_b) {
            if (cur_b->free && cur_b->size >= size) {
                if (cur_b->size >= size + sizeof(t_block) + ALIGNMENT)
                    split_block(cur_b, size);
                cur_b->free = 0;
                return cur_b;
            }
            cur_b = cur_b->next;
        }
        cur = cur->next;
    }
    return NULL;
}