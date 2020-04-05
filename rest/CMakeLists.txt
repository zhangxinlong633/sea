cmake_minimum_required (VERSION 2.8.7)

project(rest)

find_package(nng CONFIG REQUIRED)

link_directories("/usr/local/lib")
link_libraries(sea_block sea_controller)


add_executable(rest-server server.c)
target_link_libraries(rest-server nng::nng)
