cmake_minimum_required(VERSION 3.15)
project(bench_9)

set(CMAKE_CXX_STANDARD 14)

link_directories("/usr/local/lib")
link_libraries(sea_block)
link_libraries(array)
link_libraries(graph)
link_libraries(block_word)

add_executable(bench_array_test
        array/bench_array.c)

add_executable(bench_block_test
        block/bench_block.c)

add_executable(bench_graph_test
        graph/bench_graph.c)

add_executable(bench_word_test
        word/bench_word.c)
