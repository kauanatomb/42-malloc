#include "malloc.h"

void    *malloc(size_t size) {
    if (size == 0)
        return NULL;

    t_zone *zone;
    size_t aligned_size = align_size(size);
    t_zone_type zone_type = get_zone_type(aligned_size);

    if (zone_type == LARGE) {
        zone = create_zone(sizeof(t_zone) + sizeof(t_block) + aligned_size,
                        zone_type);
        if (!zone)
            return NULL;
        return get_block_payload(zone->blocks);
    }
    zone = get_zones_list(zone_type);
    t_block *found = NULL;

    while (zone) {
        found = find_free_block(zone, aligned_size);
        if (found)
            return get_block_payload(found);
        zone = zone->next;
    }
    zone = create_zone(get_zone_size(zone_type), zone_type);
    add_zone_to_list(zone, zone_type);
    return get_block_payload(zone->blocks);
    /* 
        2. run on zones of the type found
        3. run blocks inside such zone
        4. try to reuse block that is free
            4.1 use block or split it
        5. if faile:
            5.1 get zone size (for create zone)
            5.2 create zone
    */
}