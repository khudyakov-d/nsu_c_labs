#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "http_parser_callbacks.h"
#include "http_parser.h"
#include "info.h"

#define DEFAULT_PORT 80


int on_headers_complete(http_parser *_) {
    request_info_t *info = (struct request_info_t *) _->data;

    printf("%d.%d\n", _->http_major, _->http_minor);
    info->method = (char *) malloc(sizeof(http_method_str(_->method)));
    strcpy(info->method, http_method_str(_->method));

    return 0;
}

char *get_line(const char *src, int pos, int length) {
    char *line = (char *) malloc(sizeof(char) * (length + 1));
    int k = 0;

    for (int i = pos; i < pos + length; ++i) {
        line[k++] = src[i];
    }

    line[k] = 0;
    return line;
}


int on_url(http_parser *_, const char *at, size_t length) {
    static struct http_parser_url parser_url;
    http_parser_url_init(&parser_url);

    request_info_t *info = (struct request_info_t *) _->data;

    info->url = (char *) malloc((length + 1) * sizeof(char));
    memcpy(info->url, at, length);
    info->url[length] = 0;
    printf("Header field: %s\n", info->url);

    int result = http_parser_parse_url(info->url, strlen(info->url), 0, &parser_url);
    if (result != 0) {
        info->hostname = NULL;
        printf("Parse error : %s\n", info->url);
        return result;
    } else {
        info->hostname = get_line(info->url, parser_url.field_data[1].off, parser_url.field_data[1].len);
        char *port = get_line(info->url, parser_url.field_data[2].off, parser_url.field_data[2].len);

        if (strcmp(port, "") == 0) {
            info->port = DEFAULT_PORT;
        } else {
            info->port = atoi(port);
        }
    }

    return 0;
}

int on_header_field(http_parser *_, const char *at, size_t length) {
    printf("Header field: %.*s\n", (int) length, at);
    return 0;
}

int on_header_value(http_parser *_, const char *at, size_t length) {
    printf("Header value: %.*s\n", (int) length, at);
    return 0;
}

int on_body(http_parser *_, const char *at, size_t length) {
    //printf("Body: %.*s\n", (int) length, at);
    return 0;
}



