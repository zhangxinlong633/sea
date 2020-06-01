cmake_minimum_required(VERSION 3.15)
project(uint_test_block)

set(CMAKE_CXX_STANDARD 14)

link_directories("/usr/local/lib")
link_libraries(sea_block)
link_libraries(array)
link_libraries(graph)
link_libraries(block_word)

add_executable(array_test
        array/test/array_test.c)

add_executable(time_test
        time/test/test.c)

add_executable(graph_test
        graph/test/test.c)

add_executable(graph_test2
        graph/test/test_graph.c)

add_executable(word_test
        word/test/word_test.c)
