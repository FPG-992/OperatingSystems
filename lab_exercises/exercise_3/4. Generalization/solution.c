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



pid_t *child_pid;
int N;
pid_t pid;


void sig_handler(int signal) {
    if (signal == SIGTERM) {
        for (int i=0; i<N; ++i) {
            printf("%d ", child_pid[i]);
        }
        printf("\n");

        for (int i=0; i<N; ++i) {
            printf("[PARENT/PID=%d] Waiting for %d children to exit\n", pid, N-i);

            if (child_pid[i] == 0) {
                exit(0);
            }

            printf("[PARENT/PID=%d] Child with PID=%d terminated successfully\n", pid, child_pid[i]);
        }

        printf("[PARENT/PID=%d] All children exited, terminating as well\n", pid);
        exit(0);
    }
}


void set_signal_action() {
	struct sigaction act;

	bzero(&act, sizeof(act));

	act.sa_handler = &sig_handler;

    sigaction(SIGTERM, &act, NULL);
}



int main(int argc, char* argv[]) {
    // 0 for random and 1 for round-robin
    int mode = 1;

    // Argument Conditions
    if (argc != 3 && argc != 2) {
        printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
        return 1;
    }
    else if (!is_digit(argv[1]) || argv[1][0] == '-') {
        printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
        return 1;
    }
    else if (argc == 3) {
        if (!strcmp(argv[2], "--random")) {
            mode = 0;
        }
        else if (!strcmp(argv[2], "--round-robin")) {
            mode = 1;
        }
        else {
            printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
            return 1;
        }
    }

    set_signal_action();

    N = string_to_int(argv[1]);
    
    child_pid = (pid_t *) malloc(N * sizeof(pid_t));
    if (child_pid == NULL) {
        errExit("malloc");
    }

    char c_read_buffer[N][10];
    char p_read_buffer[N][10];
    char i_read_buffer[10];
    char message[N][10];

    int k;

    int fds[N][2][2];

    // Setting the `poll` structure which will allow us to wait before reading something
    struct pollfd p2c_pfds[N][1]; // Communications from the parent to the child
    struct pollfd c2p_pfds[N][1]; // Communication from the child to the parent

    // Iterating to create fds for all childer
    for (int i=0; i<N; ++i) {
        // Creating 2 file descriptors for two way communication
        for (int j=0; j<2; ++j) {
            if (pipe(fds[i][j])) {
                errExit("pipe");
            }
        }
        p2c_pfds[i][0].fd = fds[i][p2c][READEND];
        p2c_pfds[i][0].events = POLLIN;

        c2p_pfds[i][0].fd = fds[i][c2p][READEND];
        c2p_pfds[i][0].events = POLLIN;
    }

    // Communication from the stdin to the parent
    struct pollfd i2p_pfds[1];
    i2p_pfds[0].fd = 0;
    i2p_pfds[0].events = POLLIN;

    // Creating the 'N' child processes
    for (int i=0; i<N; ++i) {
        if ((child_pid[i] = fork()) == -1) {
            errExit("fork");
        }
    }

    for (int i=0; i<N; ++i) {
        if (child_pid[i] == 0) {
            // Child's code

            // Closing the connection we don't need
            close(fds[i][p2c][WRITEEND]);
            close(fds[i][c2p][READEND]);

            while (1) {
                // Wait untill all the parent has succesfully sent a message to the child
                poll(p2c_pfds[i], 1, 0);
                if (p2c_pfds[i][0].revents & POLLIN) {
                    read(fds[i][p2c][READEND], c_read_buffer[i], sizeof(c_read_buffer[i]));
                    printf("[Child=%d, pid=%d] Received number: %s\n", i, getpid(), c_read_buffer[i]);

                    // Wait for 10 seconds
                    sleep(5);

                    // Decrement the number
                    int k = string_to_int(c_read_buffer[i]) - 1;

                    // Send message to the parent
                    strcpy(message[i], int_to_string(k));
                    write(fds[i][c2p][WRITEEND], message[i], (strlen(message[i]) + 1));
                }
            }
        }
    }

    // Parent's Code
    // Every child is going to be captured in the infinite while loop

    // Closing the connections we don't need
    for (int i=0; i<N; ++i) {
        close(fds[i][p2c][READEND]);
        close(fds[i][c2p][WRITEEND]);

        k = i+1;

        // Send message to child
        strcpy(message[i], int_to_string(k));
        write(fds[i][p2c][WRITEEND], message[i], (strlen(message[i]) + 1));
    }

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
                if (kill((pid = getpid()), SIGTERM) == -1) {
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

        for (int i=0; i<N; ++i) {
            // Waiting for the child to sent back a message
            poll(c2p_pfds[i], 1, 0);
            if (c2p_pfds[i][0].revents & POLLIN) {
                read(fds[i][c2p][READEND], p_read_buffer[i], sizeof(p_read_buffer[i]));

                k = string_to_int(p_read_buffer[i]);
                printf("[Parent] Received number: %d from child %d (pid=%d)\n", k, i, child_pid[i]);

                strcpy(message[i], int_to_string(k));
                write(fds[i][p2c][WRITEEND], message[i], (strlen(message[i]) + 1));
            }
        }
    }

    for (int i=0; i<N; ++i) {
        close(p2c_pfds[i][0].fd);
        close(c2p_pfds[i][0].fd);
    }

    free(child_pid);

    return 0;
}
