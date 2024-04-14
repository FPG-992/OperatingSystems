#define _XOPEN_SOURCE 700 // resolves the `struct sigaction sa` incomplete type error

#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#include "functions.c"

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define c2p 0
#define p2c 1
#define READEND 0
#define WRITEEND 1


// Global variables which we'll use both on signal handler and inside of main
pid_t *child_pid;
int N;
pid_t pid;


void sig_handler(int signal) {
    if (signal == SIGTERM) {
        // Iterating to terminate each child first
        for (int i=0; i<N; ++i) {
            printf("[PARENT/PID=%d] Waiting for %d children to exit\n", pid, N-i);

            if (child_pid[i] == 0) {
                exit(0);
            }

            printf("[PARENT/PID=%d] Child with PID=%d terminated successfully\n", pid, child_pid[i]);
        }

        // Terminating the parent process
        printf("[PARENT/PID=%d] All children exited, terminating as well\n", pid);
        exit(0);
    }
}


void set_signal_action() {
    // Declaring the sigaction structure
	struct sigaction act;

    // Setting all of the structure's bits to 0 to avoid errors relating to uninitialized variables
	bzero(&act, sizeof(act));
    
    // Setting the signal handler as the default action
	act.sa_handler = &sig_handler;

    // Applying the action to the SIGTERM signal
    sigaction(SIGTERM, &act, NULL);
}



int main(int argc, char* argv[]) {
    // 0 for random and 1 for round-robin
    int mode = 1;

    // Argument Conditions
    if ((argc != 3 && argc != 2) || (!is_digit(argv[1]) || argv[1][0] == '-')) {
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

    // Setting the seed to the random number generator
    srand(time(NULL));

    // Setting the signal handler
    set_signal_action();

    // Initializing the array which will contain all the childeren pids
    // We need to do this in a dynamic way because we also need `child_pid` in the signal handler
    N = string_to_int(argv[1]);
    child_pid = (pid_t *) malloc(N * sizeof(pid_t));
    if (child_pid == NULL) {
        errExit("malloc");
    }

    char c_read_buffer[N][10]; // buffer that will be used from the children to read from parent
    char p_read_buffer[N][10]; // buffer that will be used from the parent to read from children
    char i_read_buffer[10];    // buffer that will be used from the parent to read from the stdin
    
    char message[N][10];       // the message that will be used to communicte through the pipes

    int number;                // number which will be used in the parent-child communication
    
    int child_id = -1;         // pid of the child that will get the next job from parent

    // Array of the file descriptors that will be used in `pipe` and in the `poll` structure
    int fds[N][2][2];

    // Setting the `poll` structure which will allow an object to wait before reading or writing something
    struct pollfd p2c_pfds[N][1]; // communication from the parent to the children
    struct pollfd c2p_pfds[N][1]; // communication from the children to the parent

    // Iterating to create fds for all children
    for (int i=0; i<N; ++i) {
        // Creating 2 file descriptors for two way communication
        for (int j=0; j<2; ++j) {
            if (pipe(fds[i][j])) {
                errExit("pipe");
            }
        }
        // Passing the parameters to each `poll` object
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

    // Iterating for every child to execute it's code
    for (int i=0; i<N; ++i) {
        if (child_pid[i] == 0) {
            // Child's code

            // Closing the connection we don't need
            close(fds[i][p2c][WRITEEND]);
            close(fds[i][c2p][READEND]);

            while (1) {
                // Wait untill the parent has succesfully sent a message to the child i
                if (poll(p2c_pfds[i], 1, 0) == -1) { 
                    errExit("poll");
                }
                if (p2c_pfds[i][0].revents & POLLIN) {
                    read(fds[i][p2c][READEND], c_read_buffer[i], sizeof(c_read_buffer[i]));
                    printf("[Child=%d, pid=%d] Received number: %s\n", i, getpid(), c_read_buffer[i]);

                    // Wait for 10 seconds
                    sleep(10);

                    // Decrement the number
                    number = string_to_int(c_read_buffer[i]) - 1;

                    // Send message to the parent
                    strcpy(message[i], int_to_string(number));
                    write(fds[i][c2p][WRITEEND], message[i], (strlen(message[i]) + 1));
                }
            }
        }
    }

    // Parent's Code
    // Every child is going to be captured in the infinite while loop

    for (int i=0; i<N; ++i) {
        // Closing the connections we don't need
        close(fds[i][p2c][READEND]);
        close(fds[i][c2p][WRITEEND]);

        // The random number that the parent will send to the child i
        number = random_between(-9, 9);

        // Send message to child
        strcpy(message[i], int_to_string(number));
        write(fds[i][p2c][WRITEEND], message[i], (strlen(message[i]) + 1));
    }

    // Capturing the parent pid
    pid = getpid();

    // The main code of the parent processpi
    while (1) {
        // Checking if the user typed something in the stdin
        if (poll(i2p_pfds, 1, 0) == -1) {
            errExit("poll");
        }
        if (i2p_pfds[0].revents & POLLIN) {
            read(i2p_pfds[0].fd, i_read_buffer, sizeof(i_read_buffer));

            // stdin also read the '\n'
            i_read_buffer[strlen(i_read_buffer)-1] = '\0';

            if (!strcmp(i_read_buffer, "help")) {
                printf("[Parent, pid=%d] Type a number to send job to a child\n", pid);
            }
            else if (!strcmp(i_read_buffer, "exit")) {
                if (kill(pid, SIGTERM) == -1) {
                    return 1;
                }
            }
            else if (is_digit(i_read_buffer)) {
                int task_to;
                if (mode == 1) {
                    // round-robin
                    child_id = (child_id + 1) % N;
                    task_to = child_pid[child_id];
                }
                else {
                    // random
                    child_id = random_between(0, N-1);
                    task_to = child_pid[child_id];
                }
                printf("[Parent, pid=%d] Assigned %s to child %d (pid=%d)\n", pid, i_read_buffer, child_id, task_to);
            }
            else {
                printf("[Parent, pid=%d] Type a number to send job to a child\n", pid);
            }
        }

        for (int i=0; i<N; ++i) {
            // Checking if any child has sent a message to the parent
            if (poll(c2p_pfds[i], 1, 0) == -1) {
                errExit("poll");
            }
            if (c2p_pfds[i][0].revents & POLLIN) {
                read(fds[i][c2p][READEND], p_read_buffer[i], sizeof(p_read_buffer[i]));

                number = string_to_int(p_read_buffer[i]);
                printf("[Parent, pid=%d] Received number: %d from child %d (pid=%d)\n", pid, number, i, child_pid[i]);

                // Sending back message to the child i
                strcpy(message[i], int_to_string(number));
                write(fds[i][p2c][WRITEEND], message[i], (strlen(message[i]) + 1));
            }
        }
    }

    // Closing all the connections
    for (int i=0; i<N; ++i) {
        close(p2c_pfds[i][0].fd);
        close(c2p_pfds[i][0].fd);
    }

    // Deallocating the memeory space of the `child_pid` array
    free(child_pid);

    return 0;
}
