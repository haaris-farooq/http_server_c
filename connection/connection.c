#include "connection.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

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

    int result = select(server_socket + 1, &read_fds, NULL, NULL, &timeout);
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
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket < 0)
        {
            perror("Error accepting connection");
            return -1;
        }
    }

    // Buffer to store the HTTP request
    char request[2048];

    // Read the HTTP request
    ssize_t bytes_received = recv(client_socket, request, sizeof(request) - 1, 0);
    if (bytes_received < 0)
    {
        perror("Error reading request");
        return -1;
    }

    // Null-terminate the request
    request[bytes_received] = '\0';

    // Print the request
    printf("Received request:\n%s\n", request);

    // Define the HTTP response
    char *http_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
                      "<!DOCTYPE html>"
                      "<html>"
                      "<head>"
                      "<title>Page Title</title>"
                      "</head>"
                      "<body>"
                      "<h1>This is a Heading</h1>"
                      "<p>This is a paragraph.</p>"
                      "<p>This is another paragraph.</p>"
                      "</body>"
                      "</html>";

    // Send the HTTP response to the client
    if (send(client_socket, http_response, strlen(http_response), 0) < 0)
    {
        perror("Error sending response");
        return -1;
    }

    // Close the connection
    close(client_socket);

    return 0;
}

// Handle a connection from a client
void handle_connection(int client_socket)
{
    char buffer[1024] = {0};
    char *http_response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    read(client_socket, buffer, 1024);
    printf("%s\n", buffer);
    write(client_socket, http_response, strlen(http_response));
    close(client_socket);
}