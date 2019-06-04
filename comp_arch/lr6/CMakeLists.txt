cmake_minimum_required(VERSION 3.12)
project(lr6)

set(CMAKE_CXX_STANDARD 17)


add_executable(lr6 main.cpp main.cpp)
target_link_libraries(lr6  usb-1.0)

