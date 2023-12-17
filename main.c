#include "server/server.h"
#include "connection/connection.h"
#include <stdio.h>

#define PORT 8080

int main()
{
    // Start the server loop
    if (start_server(PORT) < 0)
    {
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }

    return 0;
}