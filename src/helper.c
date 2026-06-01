#include "malloc.h"

// alignment architectural 
size_t  align_size(size_t size) {
    return (size + (ALIGNMENT - 1) & ~(ALIGNMENT - 1));
}

// zone cannot be too small
size_t  get_zone_size(t_zone_type type) {
    size_t page = getpagesize();

    if (type == TINY)
        return page * 32;
    if (type == SMALL)
        return page * 128;
    return page * 256;
}

// allocation strategy
t_zone_type get_zone_type(size_t size) {
    if (size <= TINY_MAX)
        return TINY;
    else if (size <= SMALL_MAX)
        return SMALL;
    else
        return LARGE;
}

t_zone **get_zones_list(t_zone_type type) {
    if (type == TINY)
        return &g_malloc.tiny_zones;

    if (type == SMALL)
        return &g_malloc.small_zones;

    return &g_malloc.large_zones;
}

// create zone with mmap and initial block
t_zone  *create_zone(size_t zone_size, t_zone_type type) {
    t_zone *zone = mmap(NULL, zone_size, PROT_READ | PROT_WRITE,
        MAP_ANON | MAP_PRIVATE, -1 , 0);
    if (zone == MAP_FAILED)
        return NULL;
    zone->size = zone_size;
    zone->type = type;
    zone->next = NULL;
    zone->blocks = NULL;

    t_block *block = (t_block *)(zone + 1);
    block->size = zone_size - sizeof(t_zone) - sizeof(t_block);
    block->free = 1;
    block->next = NULL;
    zone->blocks = block;
    return zone;
}