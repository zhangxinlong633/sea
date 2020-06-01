cmake_minimum_required(VERSION 3.15)
project(sea C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(src/block/base)
add_subdirectory(src/block/array)
add_subdirectory(src/block/graph)
add_subdirectory(src/block/word)

add_subdirectory(src/controller)
add_subdirectory(src/rest)

add_subdirectory(test/benchmark)

add_subdirectory(test/block/base/test)
add_subdirectory(test/block/array/test)
add_subdirectory(test/block/graph/test)
add_subdirectory(test/block/word/test)

add_subdirectory(test/controller/test)

