cmake_minimum_required(VERSION 3.15)
project(array_test)

set(CMAKE_CXX_STANDARD 14)

link_directories("/usr/local/lib")
link_libraries(array)

add_executable(array_test
        array_test.c)