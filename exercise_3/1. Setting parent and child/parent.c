#define _XOPEN_SOURCE 700 // resolves the `struct sigaction sa` incomplete type error

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>


pid_t child_pid;
pid_t pid;


void sig_handler(int signal) {
    if (signal == SIGTERM) {
        printf("[PARENT] Waiting for 1 children to exit\n");
        
        kill(child_pid, SIGTERM);
        
        if (waitpid(child_pid, 0, 0) == -1) {
            exit(1);
        }

        printf("[PARENT] All children exited, terminating as well\n");
        exit(0);
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


int main(int argc, char* argv[]) {
    set_signal_action();

    pid = getpid();

    int status;
    int children_pids[100];
    int N;

    if ((child_pid = fork()) == -1) {
        printf("Error: Couldn't create the child-process\n");
        return 1;
    }

    char child_procees[] = "./child";
    int index = 0;
    if (child_pid == 0) {
        // Child's Code
        printf("[PARENT] Created child %d (PID=%d)\n", index, child_pid);

        char child_id[50];
        sprintf(child_id, "%d", index);

        char *args[] = {child_procees, child_id, NULL};
        if (execv(child_procees, args) == -1) {
            printf("Error: child.c file can't be executed.\n");
            return 1;
        }
    }
    else {
        // Parent's Code

        char input[100];
        while (1) {
            printf("[Parent] Enter command: ");
            scanf("%s", input);

            if (!strcmp(input, "help")) {
                printf("[Parent] Type a number to send job to a child!\n");
            }
            else if (!strcmp(input, "exit")) {
                if (kill(pid, SIGTERM) == -1) {
                    return 1;
                }
            }
            else if (is_digit(input)) {
                printf("[Parent] Sending job %s to child.\n", input);
            }
            else {
                printf("[Parent] Type a number to send job to a child!\n");
            }
            
        }
    }

    return 0;
}
