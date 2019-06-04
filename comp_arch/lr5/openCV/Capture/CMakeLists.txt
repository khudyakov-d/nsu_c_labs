cmake_minimum_required(VERSION 3.12)
project(Capture)

set(CMAKE_CXX_STANDARD 14)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(Capture main.cpp)
target_link_libraries( Capture ${OpenCV_LIBS} )
