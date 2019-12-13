#include <malloc.h>
#include <memory.h>
#include <pthread.h>

#include "cache.h"

pthread_rwlock_t cache_rwlock = PTHREAD_RWLOCK_INITIALIZER;

int get_cache_line_number(cache_line_t *cache, char *url, cache_search_status_t *status) {
    pthread_rwlock_rdlock(&cache_rwlock);

    for (int i = 0; i < cache_lines_count; ++i) {
        pthread_rwlock_rdlock(&cache[i].rwlock);
        if (0 == strcmp(cache[i].url, url) &&
            (FILLING == cache[i].state || FINISHED == cache[i].state) &&
            PERMANENT == cache[i].memory_status) {

            *status = FOUND;
            pthread_rwlock_unlock(&cache[i].rwlock);
            return i;
        }
        pthread_rwlock_unlock(&cache[i].rwlock);
    }

    *status = CREATED;
    int cache_line_num = create_cache_line(cache, url);
    pthread_rwlock_unlock(&cache_rwlock);

    return cache_line_num;
}

int create_cache_line(cache_line_t *cache, char *url) {

    cache[cache_lines_count].url = malloc(sizeof(char) * (strlen(url) + 1));
    if (NULL == cache[cache_lines_count].url) {
        return -1;
    }

    memcpy(cache[cache_lines_count].url, url, strlen(url));
    cache[cache_lines_count].url[strlen(url)] = 0;
    cache[cache_lines_count].buf = NULL;
    cache[cache_lines_count].state = ADDED;
    cache[cache_lines_count].memory_status = NONE;

    pthread_rwlock_init(&cache[cache_lines_count].rwlock, NULL);

    int cache_num = cache_lines_count++;

    return cache_num;
}

cache_state_t get_cache_line_status(cache_line_t *cache, int num) {
    cache_state_t state = cache[num].state;
    return state;
}

void set_cache_line_status(cache_line_t *cache, int num, enum cache_state_t state) {
    cache[num].state = state;
}

int check_response_status(const char *buf) {
    if ('2' == buf[9] && '0' == buf[10] && '0' == buf[11]) return 1;
    else return 0;
}

int set_cache_memory_status(char *buf, cache_line_t *cache, int num) {
    cache[num].state = FILLING;
    if (check_response_status(buf)) {
        cache[num].memory_status = PERMANENT;
        return 0;
    } else {
        cache[num].memory_status = TEMPORAL;
        return 1;
    }
}


void drop_cache_line(cache_line_t *cache, int num) {
    cache[num].state = DROPPED;
    free(cache[num].buf);
    free(cache[num].url);
    pthread_rwlock_destroy(&cache[num].rwlock);
}

int update_cache_line(cache_line_t *cache, int cache_line_num, char buf[], size_t buf_size) {
    cache[cache_line_num].buf = (char *) realloc(cache[cache_line_num].buf, cache[cache_line_num].size + buf_size);
    if (NULL == cache[cache_line_num].buf) {
        return -1;
    }
    memcpy(cache[cache_line_num].buf + cache[cache_line_num].size, buf, buf_size);
    cache[cache_line_num].size += buf_size;
    return 0;
}