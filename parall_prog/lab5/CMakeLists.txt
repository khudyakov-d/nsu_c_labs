cmake_minimum_required(VERSION 3.12)
project(lab5 C)

set(CMAKE_C_STANDARD 99)


add_executable(lab5 main.c)
find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(lab5 ${MPI_LIBRARIES})
target_link_libraries(lab5 m)
set(PLATFORM_SPECIFIC_LIBS "-lpthread")


find_package (Threads)
target_link_libraries (lab5 ${CMAKE_THREAD_LIBS_INIT})