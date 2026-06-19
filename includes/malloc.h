#ifndef MALLOC_H
# define MALLOC_H

# include <sys/mman.h>
# include <pthread.h>
#include "libft.h"

# define ALIGNMENT 16

# define TINY_MAX 64
# define SMALL_MAX 1024

# define TINY_ZONE_PAGES 32
# define SMALL_ZONE_PAGES 128

typedef enum e_zone_type
{
    TINY,
    SMALL,
    LARGE
}   t_zone_type;

typedef struct s_block
{
    size_t          size;
    int             free;
    struct s_block  *next;
    struct s_block  *prev;
}   t_block;

typedef struct s_zone
{
    size_t              size;
    t_zone_type         type;
    struct s_zone       *next;
    t_block             *blocks;
}   t_zone;

typedef struct s_malloc
{
    t_zone  *tiny_zones;
    t_zone  *small_zones;
    t_zone  *large_zones;
}   t_malloc;

extern t_malloc g_malloc;
extern pthread_mutex_t g_malloc_lock;

/*
** implementations
*/
void    *malloc(size_t size);
void    check_leaks(void);
void    *realloc(void *ptr, size_t size);
void    show_alloc_mem(void);
void    free(void *ptr);
void    cleanup_zone_if_empty(t_zone *zone);

/*
** zone
*/
t_zone  *create_zone(size_t zone_size, t_zone_type type);

/*
** block
*/
t_block *find_free_block(t_zone *zone, size_t size);
void    split_block(t_block *block, size_t size);
void    *get_block_payload(t_block *block);
t_zone  **get_zones_list(t_zone_type type);
t_zone  *block_to_zone(t_block *block);

/*
** utils
*/
size_t  align_size(size_t size);
size_t  get_zone_size(t_zone_type type);
t_zone_type get_zone_type(size_t size);
int     is_valid_ptr(void *ptr) ;

#endif