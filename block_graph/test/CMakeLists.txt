cmake_minimum_required(VERSION 3.15)
project(graph_test)

set(CMAKE_CXX_STANDARD 14)

link_directories("/usr/local/lib")
link_libraries(graph)

add_executable(graph_data_test
        test.c)

add_executable(graph_test
        test_graph.c)