#define _XOPEN_SOURCE 700 // resolves the `struct sigaction sa` incomplete type error

#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define c2p 0
#define p2c 1
#define READEND 0
#define WRITEEND 1


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


int is_digit(const char* input) {
    int i = 0;
    while (i < strlen(input)) {
        if ((int) input[i] < 48 || (int) input[i] > 57) {
            return 0;
        }
        ++i;
    }

    return 1;
}


int string_to_int(const char *str) {
    int result = 0;
    int sign = 1; // Sign of the number, initialized as positive
    
    // Handle leading whitespace
    while (*str == ' ' || *str == '\t' || *str == '\n')
        str++;
    
    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convert each character to integer
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    // Apply sign
    result *= sign;
    
    return result;
}


// Function to get the number of digits in an integer
int num_digits(int num) {
    int count = 0;
    if (num == 0)
        return 1;
    while (num != 0) {
        num /= 10;
        count++;
    }
    return count;
}


// Function to convert an integer to a string
char *int_to_string(int num) {
    int sign = 1;
    if (num < 0) {
        sign = -1;
        num *= sign;
    }

    int n = num_digits(num);
    int i = n - 1; // Start from the end of the string
    char *str = (char*) malloc((n + 1 + (sign == -1 ? 1 : 0)) * sizeof(char)); // +1 for '\0', +1 for negative sign if needed
    
    if (str == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    // Handle negative numbers
    if (sign == -1) {
        str[0] = '-';
        i ++;
    }
    
    // Convert each digit to character
    while (num != 0) {
        str[i] = (num % 10) + '0';
        num /= 10;
        i--;
    }
    
    // Add null terminator
    str[n + (sign == -1 ? 1 : 0)] = '\0';
    
    return str;
}


int main(int argc, char* argv[]) {
    set_signal_action();

    pid = getpid();

    int status;
    int children_pids[100];
    int N;

    char c_read_buffer[10];
    char p_read_buffer[10];
    char message[10];

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
            poll(p2c_pfds, 1, -1);
            if (p2c_pfds[0].revents & POLLIN) {
                read(fds[p2c][READEND], c_read_buffer, sizeof(c_read_buffer));
                printf("[Child] Received number: %s\n", c_read_buffer);
            }

            // Wait for 10 seconds
            sleep(2);

            // Decrement the number
            int i = string_to_int(c_read_buffer) - 1;

            // Send message to the parent
            strcpy(message, int_to_string(i));
            write(fds[c2p][WRITEEND], message, (strlen(message) + 1));
        }
    }
    else {
        // Parent's Code

        // Closing the connections we don't need
        close(fds[p2c][READEND]);
        close(fds[c2p][WRITEEND]);

        int i = 9;
        while (1) {
            // Sending a message to the child
            strcpy(message, int_to_string(i));
            printf("%ld\n", write(fds[p2c][WRITEEND], message, (strlen(message) + 1)));

            // Waiting for the child to sent back a message
            poll(c2p_pfds, 1, -1);
            if (c2p_pfds[0].revents & POLLIN) {
                read(fds[c2p][READEND], p_read_buffer, sizeof(p_read_buffer));
            }

            i = string_to_int(p_read_buffer);

            printf("[Parent] Received number: %d\n\n", i);
        }
    }

    close(p2c_pfds[0].fd);
    close(c2p_pfds[0].fd);

    return 0;
}
