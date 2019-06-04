cmake_minimum_required(VERSION 3.12)
project(lab2_1 C)

set(CMAKE_C_STANDARD 99)
add_executable(lab2_1 main.c main.c)
target_link_libraries(lab2_1 m)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()