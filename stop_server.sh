#!/bin/bash

server_executable="http_server_c"

pid=$(pgrep $server_executable)

if [ -n "$pid" ]; then
    kill -9 $pid
    echo "Server stopped."
else
    echo "Server is not running."
fi