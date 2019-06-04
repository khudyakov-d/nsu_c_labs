cmake_minimum_required(VERSION 3.12)
project(lab1 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab1 main.c src.c src.h)
include_directories(/usr/lib/mpich)
target_link_libraries(lab1 m)