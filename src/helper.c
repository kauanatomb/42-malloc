#include "malloc.h"

// getpagesize() on OSX, sysconf(_SC_PAGESIZE) on Linux, as required by the subject
size_t  get_page_size(void) {
#ifdef __APPLE__
    return getpagesize();
#else
    return sysconf(_SC_PAGESIZE);
#endif
}

int is_size_too_large(size_t size) {
    size_t max_size = SIZE_MAX - ALIGNMENT - sizeof(t_zone) - sizeof(t_block) - (1 << 20);

    return (size > max_size);
}

// alignment architectural 
size_t  align_size(size_t size) {
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

// zone cannot be too small
size_t  get_zone_size(t_zone_type type) {
    size_t page = get_page_size();

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
    block->prev = NULL;
    zone->blocks = block;

    return zone;
}

static int	is_ptr_in_zone(t_zone *zone, void *ptr) {
	char	*zone_start;
	char	*zone_end;

	if (!zone)
		return (0);
	
	// after struct t_zone
	zone_start = (char *)zone + sizeof(t_zone);
	// zone->size already covers the whole mmap'd region, header included
	zone_end = (char *)zone + zone->size;
	
	// ptr is inside limits?
	return ((char *)ptr >= zone_start && (char *)ptr < zone_end);
}

static int	is_valid_block(t_zone *zone, void *ptr) {
	t_block	*block;

	if (!zone || !ptr)
		return (0);

	block = zone->blocks;
	while (block) {
		// payload starts after struct t_block
		void *payload = get_block_payload(block);
		
		if (ptr == payload && !block->free)
			return (1);
		
		block = block->next;
	}
	return (0);
}

int	is_valid_ptr(void *ptr) {
	t_zone	*zone;

	if (!ptr)
		return (0);

	zone = g_malloc.tiny_zones;
	while (zone) {
        if (is_ptr_in_zone(zone, ptr) && is_valid_block(zone, ptr))
                return 1;
        zone = zone->next;
    }

	zone = g_malloc.small_zones;
	while (zone) {
        if (is_ptr_in_zone(zone, ptr) && is_valid_block(zone, ptr))
                return 1;
        zone = zone->next;
    }

	zone = g_malloc.large_zones;
	while (zone) {
        if (is_ptr_in_zone(zone, ptr) && is_valid_block(zone, ptr))
                return 1;
        zone = zone->next;
    }

	return (0);
}