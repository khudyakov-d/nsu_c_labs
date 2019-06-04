cmake_minimum_required(VERSION 3.12)
project(lr1 C)

set(CMAKE_C_STANDARD 99)

add_executable(lr1 main.c)
target_link_libraries(lr1 m)