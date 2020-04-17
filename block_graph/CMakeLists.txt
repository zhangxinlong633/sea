cmake_minimum_required(VERSION 3.15)
project(graph)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
add_subdirectory(test)
link_directories("/usr/local/lib")
link_libraries(sea_block)


add_library(graph SHARED
        graph_v3.c
        graph_v3.h)
