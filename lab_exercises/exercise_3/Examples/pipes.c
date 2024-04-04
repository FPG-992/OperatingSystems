#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main() {
    int pd[2];
    pid_t child_pid;
    int n_bytes;
    char string[] = "Hello World!\n";
    char read_buffer[80];

    pipe(pd);

    // Creating the child process and exiting if cannot create
    if ((child_pid = fork()) == -1) {
        perror("fork");
        exit(1);
    }

    if (child_pid == 0) {
        // Child's Code

        // Close read-end channel
        close(pd[0]);

        // Writing to the write-end
        write(pd[1], string, (strlen(string) + 1));
    }
    else {
        // Parent's Code

        // Closing the write-end
        close(pd[1]);

        // Reading the read-end
        n_bytes = read(pd[0], read_buffer, sizeof(read_buffer));

        printf("Received string: %s", read_buffer);
    }

    return 0;
}
