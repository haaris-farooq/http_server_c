cmake_minimum_required(VERSION 3.10)

project(http_server_c)

set(CMAKE_C_STANDARD 99)

include_directories(connection)
include_directories(server)

add_executable(http_server_c main.c connection/connection.c server/server.c)