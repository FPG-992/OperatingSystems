#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define c2p 0
#define p2c 1
#define READEND 0
#define WRITEEND 1


int main() {
    pid_t child_pid;
    char message[100];
    char p_read_buffer[100];
    char c_read_buffer[100];
    
    // Creating 2 file descriptors for two way communication
    int fds[2][2];
    for (int i=0; i<2; ++i) {
       if (pipe(fds[i])) {
           errExit("pipe");
        }
    }

    // Setting the `poll` structure which will allow us to wait before reading something
    struct pollfd p2c_pfds[1];
    p2c_pfds[0].fd = fds[p2c][READEND];
    p2c_pfds[0].events = POLLIN;

    struct pollfd c2p_pfds[1];
    c2p_pfds[0].fd = fds[c2p][READEND];
    c2p_pfds[0].events = POLLIN;


    // Creating the child process
    if ((child_pid = fork()) == -1) {
        errExit("fork");
    }

    if (child_pid == 0) {
        // Child's Code

        // Closing the connection we don't need
        close(fds[p2c][WRITEEND]);
        close(fds[c2p][READEND]);

        // Wait untill all the parent has succesfully sent a message to the child
        poll(p2c_pfds, 1, -1);
        if (p2c_pfds[0].revents & POLLIN) {
            read(fds[p2c][READEND], c_read_buffer, sizeof(c_read_buffer));
            printf("[Child] Received message: %s\n", c_read_buffer);
        }

        close(fds[p2c][READEND]);

        // Send confirmation message to the parent
        strcpy(message, "[Child] Hi parent!");
        write(fds[c2p][WRITEEND], message, (strlen(message) + 1));

        close(fds[c2p][WRITEEND]);
    }
    else {
        // Parent's Code

        // Closing the connections we don't need
        close(fds[p2c][READEND]);
        close(fds[c2p][WRITEEND]);

        // Sending a message to the parent
        strcpy(message, "[Parent] Hi child!");
        write(fds[p2c][WRITEEND], message, (strlen(message) + 1));

        close(fds[p2c][WRITEEND]);

        // Waiting for the child to sent back a message
        poll(c2p_pfds, 1, -1);
        if (c2p_pfds[0].revents & POLLIN) {
            read(fds[c2p][READEND], p_read_buffer, sizeof(p_read_buffer));
        }

        close(fds[c2p][READEND]);

        printf("[Parent] Received message: %s\n", p_read_buffer);
    }

    return 0;
}
