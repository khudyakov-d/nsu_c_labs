cmake_minimum_required(VERSION 3.12)
project(proxy2 C)

set(CMAKE_C_STANDARD 99)

add_executable(proxy2 main.c http_parser.c http_parser.h http_parser_callbacks.c http_parser_callbacks.h cache.c cache.h info.h info.c)

find_package (Threads)
target_link_libraries (proxy2 ${CMAKE_THREAD_LIBS_INIT})
