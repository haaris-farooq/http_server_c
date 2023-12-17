# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Project's name
project(http_server_c)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Add include directories
include_directories(connection)
include_directories(server)

# Add executable target with source files
add_executable(http_server_c main.c connection/connection.c server/server.c)