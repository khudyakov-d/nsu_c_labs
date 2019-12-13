#ifndef PROXY_HTTP_PARSER_CALLBACK_H
#define PROXY_HTTP_PARSER_CALLBACK_H

#include "http_parser.h"

int on_headers_complete(http_parser* _);

int on_url(http_parser* _, const char* at, size_t length);

int on_header_field(http_parser* _, const char* at, size_t length);

int on_header_value(http_parser* _, const char* at, size_t length);

int on_body(http_parser* _, const char* at, size_t length);

#endif //PROXY_HTTP_PARSER_CALLBACK_H
