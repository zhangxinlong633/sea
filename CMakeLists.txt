cmake_minimum_required(VERSION 3.15)
project(sea C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(block)
add_subdirectory(controller)
add_subdirectory(rest)
add_subdirectory(block_array)
add_subdirectory(block_word)
add_subdirectory(block_graph)
add_subdirectory(benchmark)
