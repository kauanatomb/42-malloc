#include "malloc.h"

void *realloc(void *ptr, size_t size) {
    if (!ptr)
        return malloc(size);

    if (size == 0) {
        free(ptr);
        return NULL;
    }

    pthread_mutex_lock(&g_malloc_lock);

    if (!is_valid_ptr(ptr)) {
        pthread_mutex_unlock(&g_malloc_lock);
        return NULL;
    }

    t_block *block = (t_block *)ptr - 1;
    size_t aligned = align_size(size);

    if (block->size >= aligned) {
        if (block->size >= aligned + sizeof(t_block) + ALIGNMENT)
            split_block(block, aligned);

        pthread_mutex_unlock(&g_malloc_lock);
        return ptr;
    }

    size_t old_size = block->size;
    pthread_mutex_unlock(&g_malloc_lock);

    void *new_ptr = malloc(size);
    if (!new_ptr)
        return NULL;

    size_t to_copy = old_size < aligned ? old_size : aligned;
    ft_memcpy(new_ptr, ptr, to_copy);
    free(ptr);
    return new_ptr;
}
