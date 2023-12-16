#include "server.h"
#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Function that will be executed in a separate thread
void *handle_connection_thread(void *arg) {
    struct ThreadData *threadData = (struct ThreadData *)arg;
    handle_connection(threadData->client_socket);
    free(threadData);  // Remember to free the allocated memory
    pthread_exit(NULL);
}

int start_server(int port) {
    int server_socket = init_server_socket(port);
    if (server_socket == -1) {
        fprintf(stderr, "Error initializing server socket\n");
        return -1;
    }

    while (1) {
        int client_socket = accept_connection(server_socket);
        if (client_socket == -1) {
            // Handle error and continue accepting connections
            fprintf(stderr, "Error accepting connection. Continuing...\n");
            continue;
        }

        // Create a structure to pass data to the thread
        struct ThreadData *threadData = (struct ThreadData *)malloc(sizeof(struct ThreadData));
        if (threadData == NULL) {
            fprintf(stderr, "Error allocating memory for thread data\n");
            if (close(client_socket) == -1) {
                fprintf(stderr, "Error closing client socket\n");
            }
            continue;
        }
        threadData->client_socket = client_socket;

        // Create a new thread to handle the connection
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_connection_thread, (void *)threadData) != 0) {
            fprintf(stderr, "Error creating thread. Continuing...\n");
            free(threadData);
            if (close(client_socket) == -1) {
                fprintf(stderr, "Error closing client socket\n");
            }
            continue;
        }

        // Detach the thread to allow it to clean up resources on exit
        if (pthread_detach(thread) != 0) {
            fprintf(stderr, "Error detaching thread. Continuing...\n");
        }
    }

    // Close the server socket
    if (close(server_socket) == -1) {
        fprintf(stderr, "Error closing server socket\n");
        return -1;
    }

    return 0;
}