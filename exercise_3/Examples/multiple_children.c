#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILDREN 3


int main() {
    int pipefd[NUM_CHILDREN][2];
    pid_t child_pid[NUM_CHILDREN];
    
    // Create pipes and fork children
    for (int i=0; i<NUM_CHILDREN; i++) {
        if (pipe(pipefd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        child_pid[i] = fork();

        if (child_pid[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (child_pid[i] == 0) { // Child process
            // Close unused ends of the pipe
            close(pipefd[i][1]); // Close write end
            // Perform operations in the child process
            // For example, read from its associated pipe
            char buffer[256];
            read(pipefd[i][0], buffer, sizeof(buffer));
            printf("Child %d received: %s\n", i+1, buffer);
            close(pipefd[i][0]); // Close read end in child
            exit(EXIT_SUCCESS);
        } else { // Parent process
            // Close unused ends of the pipe
            close(pipefd[i][0]); // Close read end
            // Perform operations in the parent process
            // For example, write to the child's associated pipe
            char message[] = "Hello from parent";
            write(pipefd[i][1], message, sizeof(message));
            close(pipefd[i][1]); // Close write end in parent
        }
    }
    
    // Wait for all children to finish
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }
    
    return 0;
}
