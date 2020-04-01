cmake_minimum_required(VERSION 3.15)
project(block_test)

set(CMAKE_CXX_STANDARD 14)

link_directories("/usr/local/lib")
link_libraries(sea_block)

add_executable(test
        test.c)