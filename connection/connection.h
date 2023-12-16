#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Include any necessary headers

// Function prototypes
int init_server_socket(int port);
int accept_connection(int server_socket);
void handle_connection(int client_socket);

#endif