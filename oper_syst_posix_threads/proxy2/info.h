#ifndef PROXY2_INFO_H
#define PROXY2_INFO_H

#include <monetary.h>
#include <zconf.h>
#include "cache.h"
#include "http_parser_callbacks.h"

#define BUF_SIZE 1024 * 1024

struct request_info_t {
    char *method;
    char *url;
    char *hostname;
    int port;
};
typedef struct request_info_t request_info_t;

enum node_state_t {
    WORKING,
    SHUTDOWN
};

typedef enum node_state_t node_state_t;

struct server_desc_t {
    char buf[BUF_SIZE];
    ssize_t buf_size;

    node_state_t s_state;

    request_info_t *info;

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int cache_line_num;
    cache_line_t *cache;
};
typedef struct server_desc_t server_desc_t;


struct client_desc_t {
    int c_socket;
    cache_line_t *cache;
};
typedef struct client_desc_t client_desc_t;

void free_info(request_info_t *info);

#endif
