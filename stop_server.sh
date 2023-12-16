#!/bin/bash

# Define the name of your server executable
server_executable="http_server_c"

# Find the process ID of the server
pid=$(pgrep $server_executable)

# If the server is running, kill it
if [ -n "$pid" ]; then
    kill -9 $pid
    echo "Server stopped."
else
    echo "Server is not running."
fi