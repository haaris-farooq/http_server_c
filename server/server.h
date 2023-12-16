#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

// Define a structure to pass data to the thread
struct ThreadData {
    int client_socket;
};

// Function that will be executed in a separate thread
void *handle_connection_thread(void *arg);

// Function to start the server
int start_server(int port);

#endif // SERVER_H