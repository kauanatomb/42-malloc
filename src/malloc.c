#include "malloc.h"

static void insert_zone_ordered(t_zone **head, t_zone *new_zone) {
    if (!*head || (uintptr_t)new_zone < (uintptr_t)*head) {
        new_zone->next = *head;
        *head = new_zone;
        return;
    }

    t_zone *cur = *head;
    while (cur->next && (uintptr_t)cur->next < (uintptr_t)new_zone)
        cur = cur->next;
    new_zone->next = cur->next;
    cur->next = new_zone;
}

static t_block *find_free_block_in_zones(t_zone *zone, size_t aligned_size) {
    while (zone) {
        t_block *found = find_free_block(zone, aligned_size);
        if (found)
            return found;
        zone = zone->next;
    }
    return NULL;
}

static t_zone *create_new_zone(t_zone_type type, size_t aligned_size) {
    size_t zone_size;

    if (type == LARGE) {
        zone_size = sizeof(t_zone) + sizeof(t_block) + aligned_size;
        size_t page = getpagesize();
        zone_size = (zone_size + page - 1) & ~(page - 1);
    } else {
        zone_size = get_zone_size(type);
    }
    return create_zone(zone_size, type);
}

void *malloc(size_t size) {
    if (size == 0)
        return NULL;

    pthread_mutex_lock(&g_malloc_lock);

    size_t aligned_size = align_size(size);
    t_zone_type type = get_zone_type(aligned_size);
    t_zone **zones = get_zones_list(type);

    t_block *found = find_free_block_in_zones(*zones, aligned_size);
    if (found) {
        pthread_mutex_unlock(&g_malloc_lock);
        return get_block_payload(found);
    }

    t_zone *zone = create_new_zone(type, aligned_size);
    if (!zone) {
        pthread_mutex_unlock(&g_malloc_lock);
        return NULL;
    }
    insert_zone_ordered(zones, zone);

    split_block(zone->blocks, aligned_size);
    zone->blocks->free = 0;

    void *ptr = get_block_payload(zone->blocks);
    pthread_mutex_unlock(&g_malloc_lock);
    return ptr;
}