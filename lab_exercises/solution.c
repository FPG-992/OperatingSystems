#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>



// Returns 0 if file doesn't exists, otherwise 1.
int file_exists (char *filename) {
    struct stat   buffer;   
    return (stat (filename, &buffer) == 0);
}


int main(int argc, char* argv[]) {
    // Argument Control
    if (argc != 2) {
        printf("Usage: ./a.out filename\n");
        return 1;
    }
    if (file_exists(argv[1])) {
        printf("Error: %s already exists\n", argv[1]);
        return 1;
    }
    if (!strcmp(argv[1], "--help")) {
        printf("Usage: ./a.out filename\n");
        return 0;
    }

    // Creating the File (write permissions)
    int fd = open(argv[1], O_CREAT | O_APPEND | O_WRONLY, 0644);
    if (fd == -1) {
        perror("Error: Couldn't create file\n");
        return 1;
    }

    int status;
    pid_t child = fork();    // Creating the Child Process
    pid_t pid = getpid();    // Getting proccess' pid
    pid_t ppid = getppid();  // Getting proccess' ppid

    // The message that will be written to the file
    char message[50];

    if (child < 0) {
        close(fd);
        printf("Error: Couldn't create the child-process\n");
        return 1;
    }
    
    if (child == 0) {
        // Create the message
        sprintf(message, "[CHILD] getpid() = %d, getppid() = %d\n", pid, ppid);

        // Write the message
        if (write(fd, message, strlen(message)) < strlen(message)) {
            close(fd);
            perror("Error: Couldn't write to the file\n");
            return 1;
        }

        exit(0);
    }
    else {
        // Go to the child's process
        if (wait(&status) == -1) {
            close(fd);
            printf("Error: Process has no child-proceses\n");
            return 1;
        }

        // Create the message
        sprintf(message, "[Parent] getpid() = %d, getppid() = %d\n", pid, ppid);

        // Write the message
        if (write(fd, message, strlen(message)) < strlen(message)) {
            close(fd);
            perror("Error: Couldn't write to the file\n");
            return 1;
        }

        // Everythink worked as expected

        close(fd);
    }

    return 0;
}
