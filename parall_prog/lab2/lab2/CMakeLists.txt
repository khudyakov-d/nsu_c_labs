cmake_minimum_required(VERSION 3.12)
project(lab2 C)

set(CMAKE_C_STANDARD 99)

add_executable(lab2 main.c)
target_link_libraries(lab2 m)