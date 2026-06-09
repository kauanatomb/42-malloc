#include "malloc.h"

t_malloc g_malloc = {0};
pthread_mutex_t g_malloc_lock = PTHREAD_MUTEX_INITIALIZER;