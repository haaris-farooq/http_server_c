#!/bin/bash

src_files="main.c server/server.c connection/connection.c"

out_file="build/http_server_c"

include_dirs="-Iserver -Iconnection"

mkdir -p build

rm -f build/*

gcc $src_files $include_dirs -o $out_file -lpthread

if [ $? -eq 0 ]; then
    echo "Compilation successful. Run ./$out_file to start the server."
else
    echo "Compilation failed."
fi