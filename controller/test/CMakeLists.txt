cmake_minimum_required(VERSION 3.15)
project(controller_test)

set(CMAKE_CXX_STANDARD 14)

link_directories("/usr/local/lib")
link_libraries(sea_controller)

add_executable(controller_test
        controller_test.c)