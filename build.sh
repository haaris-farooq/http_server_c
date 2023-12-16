#!/bin/bash

# Define the source files
src_files="main.c server/server.c connection/connection.c"

# Define the output file
out_file="build/http_server_c"

# Define the include directories
include_dirs="-Iserver -Iconnection"

# Create the build directory if it doesn't exist
mkdir -p build

# Clean the build directory
rm -f build/*

# Compile the source files into an executable
gcc $src_files $include_dirs -o $out_file -lpthread

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Run ./$out_file to start the server."
else
    echo "Compilation failed."
fi