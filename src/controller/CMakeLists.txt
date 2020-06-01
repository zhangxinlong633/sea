cmake_minimum_required(VERSION 3.15)
project(controller)

set(CMAKE_CXX_STANDARD 14)
find_package(nng CONFIG REQUIRED)

include_directories("/usr/local/include")
link_directories("/usr/local/lib")
link_libraries(sea_block block_word)

add_library(sea_controller SHARED
        phone_list.c
        phone_list.h
        ./dht/dht.c
        ./dht/dht.h
        dht_list.c
        dht_list.h
        controller.c
        controller.h
        super_block.c
        super_block.h
        controller_data.c
        controller_data.h)

target_link_libraries(sea_controller nng::nng)