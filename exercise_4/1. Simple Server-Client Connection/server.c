#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main(int argc, char* argv[]) {
    int listen_fd; // The file descriptor for the socket that will listen for incoming connections. 
    int conn_fd;

    // Initialize the socket endpoint for the server
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        errExit("socket");
    }

    // Initialize the socket address struct
    struct sockaddr_in servaddr;

    // Set all the bytes of 'servaddr' to zero
    memset(&servaddr, 0, sizeof(servaddr));

    // Set the necessary fields
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080); // Server port
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Connect to localhost

    // Bind the socket: link the socket fd to the socket address struct (give socket an address)
    if (bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        errExit("bind");
    }

    // Listen for incoming connections
    if (listen(listen_fd, 10) < 0) {
        errExit("listen");
    }
    /*
    The second parameter '10' is the backlog parameter. It defines the maximum length
    of the queue of pending connections. If more connection requests are received than
    the backlog allows, the kernel may refuse the new connections.
    */

    printf("Server is listening on port 8080\n");

    // Accept a connection (waiting until one connection is established)
    while (1) {
        conn_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
        if (conn_fd < 0) {
            errExit("accept");
        }

        printf("Client connected\n");

        // Handle the connection in a child process or in another thread
        // For simplicity, here we just close the connection immediately
        close(conn_fd);
    }

    // Close the listening socket
    close(listen_fd);

    printf("Server closed\n");

    return 0;
}
