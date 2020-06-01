cmake_minimum_required(VERSION 3.15)
project(array)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

link_directories("/usr/local/lib")
link_libraries(array)

add_library(block_word SHARED
        word.c
        word.h
        md5.c
        md5.h)
