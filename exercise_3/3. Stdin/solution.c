#define _XOPEN_SOURCE 700 // resolves the `struct sigaction sa` incomplete type error

#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

#include "functions.c"

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define c2p 0
#define p2c 1
#define READEND 0
#define WRITEEND 1


// Todo: [Child] Received number: 1\n[Parent] Received number: 0\n[Child] Received number:\n[Parent] Received number: -1


pid_t child_pid;
pid_t pid;


void sig_handler(int signal) {
    if (signal == SIGTERM) {
        if (child_pid == 0) {
            printf("[Child] Terminating child (pid=%d)\n", child_pid);
            exit(1);
        }
        else {
            kill(child_pid, SIGTERM);
        
            if (waitpid(child_pid, 0, 0) == -1) {
                exit(1);
            }

            printf("[PARENT] All children exited, terminating as well\n");
            exit(1);
        }
    }
}


void set_signal_action() {
    // Declare the sigaction structure
	struct sigaction act;

    // Set all of the structure's bits to 0 to avoid errors relating to uninitialized variables
	bzero(&act, sizeof(act));

    // Set the signal handler as the default action
	act.sa_handler = &sig_handler;

    // Apply actions to the signals
    sigaction(SIGTERM, &act, NULL);
}



int main(int argc, char* argv[]) {
    set_signal_action();

    pid = getpid();

    int status;
    int children_pids[100];
    int N;

    char c_read_buffer[10];
    char p_read_buffer[10];
    char i_read_buffer[10];
    char message[10];

    // Creating 2 file descriptors for two way communication
    int fds[2][2];
    for (int i=0; i<2; ++i) {
       if (pipe(fds[i])) {
           errExit("pipe");
        }
    }

    // Setting the `poll` structure which will allow us to wait before reading something
    // Communications from the parent to the child
    struct pollfd p2c_pfds[1];
    p2c_pfds[0].fd = fds[p2c][READEND];
    p2c_pfds[0].events = POLLIN;

    // Communication from the child to the parent
    struct pollfd c2p_pfds[1];
    c2p_pfds[0].fd = fds[c2p][READEND];
    c2p_pfds[0].events = POLLIN;

    // Communication from the stdin to the parent
    struct pollfd i2p_pfds[1];
    i2p_pfds[0].fd = 0;
    i2p_pfds[0].events = POLLIN;
    

    if ((child_pid = fork()) == -1) {
        errExit("fork");
    }

    if (child_pid == 0) {
        // Child's code

        // Closing the connection we don't need
        close(fds[p2c][WRITEEND]);
        close(fds[c2p][READEND]);

        while (1) {
            // Wait untill all the parent has succesfully sent a message to the child
            poll(p2c_pfds, 1, 0);
            if (p2c_pfds[0].revents & POLLIN) {
                read(fds[p2c][READEND], c_read_buffer, sizeof(c_read_buffer));
                printf("[Child] Received number: %s\n", c_read_buffer);

                // Wait for 10 seconds
                sleep(5);

                // Decrement the number
                int i = string_to_int(c_read_buffer) - 1;

                // Send message to the parent
                strcpy(message, int_to_string(i));
                write(fds[c2p][WRITEEND], message, (strlen(message) + 1));
            }
        }
    }
    else {
        // Parent's Code

        // Closing the connections we don't need
        close(fds[p2c][READEND]);
        close(fds[c2p][WRITEEND]);
        
        int i = 9;

        // Send message to child
        strcpy(message, int_to_string(i));
        write(fds[p2c][WRITEEND], message, (strlen(message) + 1));

        while (1) {
            // Checking if the user typed something in the stdin
            poll(i2p_pfds, 1, 0);
            if (i2p_pfds[0].revents & POLLIN) {
                read(i2p_pfds[0].fd, i_read_buffer, sizeof(i_read_buffer));

                // stdin also read the '\n'
                i_read_buffer[strlen(i_read_buffer)-1] = '\0';

                if (!strcmp(i_read_buffer, "help")) {
                    printf("[Parent] Type a number to send job to a child!\n");
                }
                else if (!strcmp(i_read_buffer, "exit")) {
                    if (kill(pid, SIGTERM) == -1) {
                        return 1;
                    }
                }
                else if (is_digit(i_read_buffer)) {
                    printf("[Parent] Sending job %s to child.\n", i_read_buffer);
                }
                else {
                    printf("[Parent] Type a number to send job to a child!\n");
                }
            }

            // Waiting for the child to sent back a message
            poll(c2p_pfds, 1, 0);
            if (c2p_pfds[0].revents & POLLIN) {
                read(fds[c2p][READEND], p_read_buffer, sizeof(p_read_buffer));

                i = string_to_int(p_read_buffer);
                printf("[Parent] Received number: %d\n\n", i);

                strcpy(message, int_to_string(i));
                write(fds[p2c][WRITEEND], message, (strlen(message) + 1));
            }
        }
    }

    close(p2c_pfds[0].fd);
    close(c2p_pfds[0].fd);

    return 0;
}
