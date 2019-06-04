cmake_minimum_required(VERSION 3.12)
project(lr7_2 C)

set(CMAKE_C_STANDARD 99)

add_executable(lr7_2 main.c)
target_link_libraries(lr7_2   libatlas-base-dev)
