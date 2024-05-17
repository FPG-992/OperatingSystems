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
    
    // Initialize the socket
    int domain = AF_INET;  // IPv4 Internet protocols
    int type = SOCK_STREAM; // Stream socket (TCP)

    // Initialize the socket
    int sock_fd = socket(domain, type, 0); 
    if (sock_fd < 0) {errExit("socket");}

    // Give an address to the socket
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin)); // Zero out the sockaddr_in structure

    // Set the necessary fields
    sin.sin_family = AF_INET;
    sin.sin_port = htons(0); // Let the system choose an available port
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to all available interface

    if (bind(sock_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        errExit("bind");
    }

    /*
    For demonstration purposes, we'll assume you want to connect to a server,
    so this part needs adjustment.
    */

    // Connect to a server
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr)); // Zero out the sockaddr_in structure

    // Set the necessary fields
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080); // Set the server port to 8080
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Set the server IP address

    if (connect(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        close(sock_fd); // Close the socket
        errExit("connect");
    }

    printf("Connected to the server\n");;

    close(sock_fd); // Close the socket

    return 0;
}
