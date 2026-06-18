#include "malloc.h"

void    *malloc(size_t size) {
    if (size == 0)
        return NULL;

    pthread_mutex_lock(&g_malloc_lock);

    size_t aligned_size = align_size(size);
    t_zone_type zone_type = get_zone_type(aligned_size);

    t_zone **zones = get_zones_list(zone_type);
    t_zone *zone = *zones;
    t_block *found = NULL;

    while (zone) {
        found = find_free_block(zone, aligned_size);
        if (found) {
            pthread_mutex_unlock(&g_malloc_lock);
            return get_block_payload(found);
        }
        zone = zone->next;
    }

    size_t zone_size;
    if (zone_type == LARGE) {
        zone_size = sizeof(t_zone) + sizeof(t_block) + aligned_size;
        size_t page = getpagesize();
        zone_size = (zone_size + page - 1) & ~(page - 1);
    } else {
        zone_size = get_zone_size(zone_type);
    }

    zone = create_zone(zone_size, zone_type);
    if (!zone) {
        pthread_mutex_unlock(&g_malloc_lock);
        return NULL;
    }
    zone->next = *zones;
    *zones = zone;
    split_block(zone->blocks, aligned_size);
    zone->blocks->free = 0;
    
    void *ptr = get_block_payload(zone->blocks);
    pthread_mutex_unlock(&g_malloc_lock);
    return ptr;
}