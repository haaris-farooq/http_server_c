#include "connection.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int init_server_socket(int port)
{

    int server_socket;
    struct sockaddr_in server_address;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error creating socket");
        return -1;
    }

    // Define server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to IP address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Error binding socket to IP address");
        return -1;
    }

    // Listen for connections
    if (listen(server_socket, 5) < 0)
    {
        perror("Error listening for connections");
        return -1;
    }

    return server_socket;
}

// Accept a connection from a client
int accept_connection(int server_socket)
{
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);

    // Set the timeout to 1 second
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int result;
    do {
        result = select(server_socket + 1, &read_fds, NULL, NULL, &timeout);
    } while (result == -1 && errno == EINTR);

    if (result == -1)
    {
        perror("select");
        return -1;
    }
    else if (result == 0)
    {
        // Timeout expired, no new connections
        return -1;
    }
    else
    {
        // A new connection is available, accept it
        do {
            client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        } while (client_socket == -1 && errno == EINTR);

        if (client_socket < 0)
        {
            perror("Error accepting connection");
            return -1;
        }
    }

    return client_socket;
}

// Handle a connection from a client
void handle_connection(int client_socket)
{
    char buffer[2048] = {0};
    char *http_response = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html\r\n"
                      "Connection: close\r\n\r\n"
                      "<!DOCTYPE html>"
                      "<html>"
                      "<head>"
                      "<title>Hello World</title>"
                      "</head>"
                      "<body>"
                      "<p><b>Hello world!</b></p>"
                      "</body>"
                      "</html>";

    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Error reading from socket");
        return;
    }
    buffer[bytes_read] = '\0'; // Ensure null-termination

    printf("%s\n", buffer);
    write(client_socket, http_response, strlen(http_response));
    close(client_socket);
}