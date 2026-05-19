#include "malloc.h"

void    *malloc(size_t size) {
    /* 
        1. choose type (TINY/SMALL..)
            1.1 align size
            1.2 get zone type
        2. run on zones of the type found
        3. run blocks inside such zone
        4. try to reuse block that is free
            4.1 use block or split it
        5. if faile:
            5.1 get zone size (for create zone)
            5.2 create zone
    */
}