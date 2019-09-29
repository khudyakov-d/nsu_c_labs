cmake_minimum_required(VERSION 3.12)
project(math C)

set(CMAKE_C_STANDARD 99)

add_executable(math main.c)
target_link_libraries(math m)