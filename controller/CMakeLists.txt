cmake_minimum_required(VERSION 3.15)
project(controller)

add_subdirectory(test)

set(CMAKE_CXX_STANDARD 14)

include_directories("/usr/local/include")
link_directories("/usr/local/lib")
link_libraries(sea_block)

add_library(sea_controller SHARED
        gossip_list.c
        gossip_list.h
        phone_list.c
        phone_list.h
        controller.c)

