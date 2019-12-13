#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <zconf.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <pthread.h>
#include "http_parser.h"
#include "http_parser_callbacks.h"
#include "info.h"

#define PORT 4338
http_parser_settings p_sett;
pthread_mutexattr_t mutexattr;

int exit_point = 0;
int cache_lines_count = 0;

void ts_printf(const char *string) {
    write(fileno(stdout), string, strlen(string));
}

void sig_handler(int sig) {
    exit_point = 1;
}

void init_parser() {
    p_sett.on_headers_complete = on_headers_complete;
    p_sett.on_url = on_url;
    p_sett.on_header_field = on_header_field;
    p_sett.on_header_value = on_header_value;
    p_sett.on_body = on_body;
}

int init_mutex_attr() {

    if (pthread_mutexattr_init(&mutexattr) != 0) {
        ts_printf("Couldn't init mutex attr\n");
        return -1;
    }

    if (pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK) != 0) {
        ts_printf("Couldn't set type of attr\n");
        return -1;
    }

    return 0;
}


void drop_connection(int c_socket, request_info_t *request_info) {
    close(c_socket);
    free_info(request_info);
}


int set_server_connection(request_info_t *info) {
    int server_fd;

    struct hostent *server_host;
    struct sockaddr_in s_address;

    server_host = gethostbyname(info->hostname);
    if (NULL == server_host) {
        ts_printf("Couldn't find IP host\n");
        return -1;
    }

    memset(&s_address, 0, sizeof(s_address));

    s_address.sin_family = AF_INET;
    memcpy(&s_address.sin_addr.s_addr, server_host->h_addr_list[0], sizeof(struct in_addr));
    s_address.sin_port = htons((uint16_t) info->port);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(server_fd, (struct sockaddr *) &s_address, sizeof(s_address))) {
        perror("Couldn't connect to server\n");
        return -1;
    }
    return server_fd;
}

server_desc_t *
create_server_desc(char query[], ssize_t query_size, request_info_t *info, cache_line_t **cache, int cache_line_num) {
    server_desc_t *server_desc = (server_desc_t *) malloc(sizeof(server_desc_t));

    memcpy(server_desc->buf, query, (size_t) query_size);
    server_desc->buf_size = query_size;

    server_desc->c_state = WORKING;
    server_desc->s_state = WORKING;

    server_desc->info = info;

    if (pthread_mutex_init(&server_desc->mutex, &mutexattr) != 0) {
        ts_printf("Couldn't init mutex\n");
        return NULL;
    }

    server_desc->cache_line_num = cache_line_num;
    server_desc->cache = *cache;
    return server_desc;
}

void *server_handler(void *value) {
    server_desc_t *s_desc = (server_desc_t *) value;

    ssize_t s_buf_size = BUF_SIZE;
    char s_buf[BUF_SIZE];

    pthread_mutex_lock(&s_desc->mutex);

    int s_socket = set_server_connection(s_desc->info);

    if (0 == s_socket) {
        s_desc->s_state = SHUTDOWN;
        pthread_cond_signal(&s_desc->cond);
        pthread_exit(NULL);
    } else {
        ssize_t res = write(s_socket, s_desc->buf, (size_t) s_desc->buf_size);

        if (res == -1 || res == 0) {
            s_desc->s_state = SHUTDOWN;
            pthread_cond_signal(&s_desc->cond);
            pthread_exit(NULL);
        } else {
            pthread_cond_signal(&s_desc->cond);
            pthread_mutex_unlock(&s_desc->mutex);

            while (1) {
                ssize_t rcount = read(s_socket, s_buf, (size_t) s_buf_size);

                pthread_rwlock_wrlock(&s_desc->cache[s_desc->cache_line_num].rwlock);

                if (0 == s_desc->buf_size) {
                    s_desc->s_state = SHUTDOWN;
                    if (NO_CASH != s_desc->cache_line_num) {
                        set_cache_line_status(s_desc->cache, s_desc->cache_line_num, FINISHED);
                    }

                    pthread_rwlock_unlock(&s_desc->cache[s_desc->cache_line_num].rwlock);
                    break;
                } else {
                    if (NO_CASH != s_desc->cache_line_num) {
                        if (ADDED == get_cache_line_status(s_desc->cache, s_desc->cache_line_num)) {
                            set_cache_memory_status(s_buf, s_desc->cache, s_desc->cache_line_num);
                        }

                        if (FILLING == get_cache_line_status(s_desc->cache, s_desc->cache_line_num)) {
                            if (update_cache_line(s_desc->cache,
                                                  s_desc->cache_line_num,
                                                  s_buf,
                                                  (size_t) rcount) < 0) {

                                drop_cache_line(s_desc->cache, s_desc->cache_line_num);
                                s_desc->cache_line_num = NO_CASH;
                            }
                        }
                    }
                }

                pthread_rwlock_unlock(&s_desc->cache[s_desc->cache_line_num].rwlock);
            }
        }
    }

    close(s_socket);
    pthread_exit(NULL);
}

int create_server_handler(server_desc_t *server_desc) {
    pthread_t pthread = {0};

    if (0 != pthread_create(&pthread, NULL, server_handler, server_desc)) {
        return -1;
    }
    pthread_detach(pthread);

    return 0;
}

void reading_cache_handler(int c_socket, int cache_num, cache_line_t *cache) {
    size_t cur_line_pos = 0;

    while (1) {
        pthread_rwlock_rdlock(&cache[cache_num].rwlock);
        cache_line_t cash_line = cache[cache_num];
        pthread_rwlock_unlock(&cache[cache_num].rwlock);

        if (DROPPED != cash_line.state) {
            size_t cash_buf_size = CASH_BUF_SIZE;
            size_t last_pos = cur_line_pos;
            size_t new_pos = cur_line_pos + cash_buf_size;

            if (new_pos > cash_line.size) {
                cash_buf_size = cash_line.size - last_pos;
            }

            if (cash_buf_size > 0) {
                ssize_t res = write(c_socket, cash_line.buf + last_pos, cash_buf_size);
                cur_line_pos += cash_buf_size;
                if (res == -1 || res == 0) {
                    ts_printf("Client disconnected\n");
                    break;
                }
            }

            if (FINISHED == cash_line.state && cur_line_pos == cash_line.size) {
                break;
            }

        } else {
            break;
        }
    }
}

void new_query_handler(int c_socket, server_desc_t *s_desc, int cache_num) {
    pthread_cond_wait(&s_desc->cond, &s_desc->mutex);

    if (SHUTDOWN == s_desc->s_state) {
        ts_printf("Server disconnected\n");
        pthread_exit(NULL);
    } else {
        reading_cache_handler(c_socket, cache_num, s_desc->cache);

        if(TEMPORAL == s_desc->cache[cache_num].memory_status && DROPPED != s_desc->cache[cache_num].state) {
            drop_cache_line(s_desc->cache, cache_num);
        }
    }
}


void *pthread_client_handler(void *value) {
    client_desc_t *c_desc = (client_desc_t *) value;
    server_desc_t *s_desc = NULL;

    ssize_t c_buf_size;
    char c_buf[BUF_SIZE];

    http_parser r_parser;
    http_parser_init(&r_parser, HTTP_REQUEST);

    request_info_t *info = (request_info_t *) malloc(sizeof(request_info_t));

    c_buf_size = read(c_desc->c_socket, c_buf, sizeof(c_buf));

    if (0 == c_buf_size) {
        ts_printf("Couldn't read query\n");
    } else {
        r_parser.data = info;
        size_t parsed_bytes = http_parser_execute(&r_parser, &p_sett, c_buf, (size_t) c_buf_size);

        if (parsed_bytes != c_buf_size) {
            ts_printf("Couldn't parse http message\n");
        } else {
            cache_search_status_t status;
            int cache_num = get_cache_line_number(c_desc->cache, info->url, &status);
            if (0 == strcmp(info->method, GET)) {
                switch (status) {
                    case CREATED:
                        s_desc = create_server_desc(c_buf, c_buf_size, info, &c_desc->cache, cache_num);
                        if (NULL == s_desc) {
                            ts_printf("Couldn't create server desc\n");
                        } else {
                            pthread_mutex_lock(&s_desc->mutex);
                            if (create_server_handler(s_desc) < 0) {
                                ts_printf("Couldn't create server handler\n");
                            } else {
                                new_query_handler(c_desc->c_socket, s_desc, cache_num);
                            }
                            pthread_mutex_unlock(&s_desc->mutex);
                        }
                        free(s_desc);
                        break;
                    case FOUND:
                        reading_cache_handler(c_desc->c_socket, cache_num, c_desc->cache);
                        break;
                }
            } else {
                ts_printf("Server does not support this type of query\n");
            }
        }
    }

    drop_connection(c_desc->c_socket, info);
    pthread_exit(NULL);
}


int add_connection(int client_socket, cache_line_t *cache) {
    pthread_t pthread = {0};
    client_desc_t *client_desc = (client_desc_t *) malloc(sizeof(client_desc_t));
    client_desc->c_socket = client_socket;
    client_desc->cache = cache;

    if (0 != pthread_create(&pthread, NULL, pthread_client_handler, client_desc)) {
        ts_printf("Cannot create client handler\n");
        return -1;
    }
    pthread_detach(pthread);

    return 0;
}


int main() {
    const int listen_port = PORT;
    int listen_fd;
    struct sockaddr_in server_address, client_address;
    unsigned int address_length = sizeof(client_address);


    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(listen_port);

    if (bind(listen_fd, (struct sockaddr *) &server_address, sizeof(server_address))) {
        ts_printf("Couldn't bind socket\n");
        return -1;
    }

    if (listen(listen_fd, SOMAXCONN) != 0) {
        ts_printf("Error in Listen.\n");
        return -1;
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sig_handler);
    signal(SIGKILL, sig_handler);

    init_parser();
    init_mutex_attr();

    cache_line_t *cache = (cache_line_t *) malloc(sizeof(struct cache_line_t) * CASH_SIZE);

    while (!exit_point) {
        int client_fd = accept(listen_fd, (struct sockaddr *) &client_address, &address_length);
        if (client_fd <= 0) {
            ts_printf("Couldn't accept connections\n");
            break;
        }

        if (add_connection(client_fd, cache)) {
            ts_printf("Couldn't add new connection\n");
        }
    }

    free(cache);
    close(listen_fd);
}