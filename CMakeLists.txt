cmake_minimum_required(VERSION 3.15)
project(sea C)

set(CMAKE_C_STANDARD 11)

include_directories(block)
include_directories(layout)

add_executable(sea
        block/public.h
        block/sea_block.c
        block/sea_block.h
        block/sea_block_data.c
        block/sea_block_data.h
        block/sea_block_index.c
        block/sea_block_index.h
        block/sea_block_offset.c
        block/sea_block_offset.h
        block/sea_block_record.c
        block/sea_block_record.h
        block/test.c
        block/utils.c
        block/utils.h
        layout/sea_layout.c
        layout/sea_layout.h api/api.c api/api.h http/http_fs.c http/http_fs.h http/http.c http/http.h)
