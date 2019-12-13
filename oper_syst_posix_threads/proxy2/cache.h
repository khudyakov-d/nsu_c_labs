#ifndef PROXY2_CASH_H
#define PROXY2_CASH_H

#include <glob.h>
#include <zconf.h>

#define NO_CASH -1
#define GET "GET"
#define CASH_SIZE 512
#define CASH_BUF_SIZE 1024*80

extern int cache_lines_count;
extern pthread_rwlock_t cache_rwlock;

enum cache_memory_status_t {
    NONE,
    TEMPORAL,
    PERMANENT
};
enum cache_memory_status_t cache_memory_status;

enum cache_search_status_t {
    FOUND,
    CREATED
};
typedef enum cache_search_status_t cache_search_status_t;


enum cache_state_t {
    ADDED,
    FILLING,
    FINISHED,
    DROPPED
};
typedef enum cache_state_t cache_state_t;


struct cache_line_t {
    cache_state_t state;
    enum cache_memory_status_t memory_status;
    size_t size;
    char *url;
    char *buf;
    pthread_rwlock_t rwlock;
};
typedef struct cache_line_t cache_line_t;

int get_cache_line_number(cache_line_t *cache, char *url, cache_search_status_t *status);

int create_cache_line(cache_line_t *cache, char *url);

int set_cache_memory_status(char *buf, cache_line_t *cache, int num);

void drop_cache_line(cache_line_t *cache, int num);

cache_state_t get_cache_line_status(cache_line_t *cache, int num);

void set_cache_line_status(cache_line_t *cache, int num, enum cache_state_t state);

int update_cache_line(cache_line_t *cache, int cache_line_num, char buf[], size_t buf_size);

#endif //PROXY2_CASH_H

