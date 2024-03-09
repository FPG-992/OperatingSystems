#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>



int main(int argc, char* argv[]) {
    int status;
    int wait_status;
    pid_t child = fork();
    pid_t pid = getpid();
    pid_t ppid = getppid();
    char child_procees[] = "./child";

    if (child < 0) {
        printf("Error: Couldn't create the child-process\n");
        return 1;
    }
    if (child == 0) {
        // Child's Code
        printf("[PARENT/PID=%d] Created child (PID=%d)\n", ppid, pid);

        char *args[] = {child_procees, NULL};
        if (execv(child_procees, args) == -1) {
            printf("Error: child.c file can't be executed.\n");
            return 1;
        }
    }
    else {
        // Parent's Code
        char sent_sig[50];

        while (1) {
            scanf("%s", sent_sig); // Wait for user input to sent to the child process

            // Sent SIGTERM signal to the child
            if (!strcmp(sent_sig, "SIGTERM")) {
                if (kill(child, SIGTERM) == -1) {
                    return 1;
                }
            }

            // Wait for this child to exit
            if (waitpid(child, &wait_status, 0) == -1) {
                return 1;
            }

            // Conditions about the 'health' of the child
            if (WIFSIGNALED(wait_status)){
                printf("[PARENT] Child: %d, exited from signal SIGTERM.\n", child);
                return 1;
            }
            else if (WEXITSTATUS(wait_status)) {
                printf("Exit status was: %d\n", WIFEXITED(wait_status));
                return 0;
            }
            else if (WIFEXITED(wait_status)) {
                printf("Exited Normally...\n");
                return 0;
            }
        }
    }

    return 0;
}
