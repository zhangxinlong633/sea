cmake_minimum_required(VERSION 3.15)
project(array)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
add_subdirectory(test)


add_library(array SHARED
        array.c
        array.h)
