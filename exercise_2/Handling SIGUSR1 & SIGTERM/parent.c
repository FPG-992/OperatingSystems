#define _XOPEN_SOURCE 700 // resolves the `struct sigaction sa` incomplete type error

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>


/*
    Εργαλεια

    When a child process stops or terminates, SIGCHLD is sent to the parent process.
    The default response to the signal is to ignore it. The signal can be caught and
    the exit status from the child process can be obtained by immediately calling wait.
*/


// Declearing them as global, because we need them inside `sig_handler`
int total_children;
int children_pids[100];
int N;


void sig_handler(int signal) {
    if (signal == SIGCHLD) {
        --total_children;
    }
    else if (signal == SIGUSR1) {
        for (int i=0; i<N; ++i) {
            kill(children_pids[i], SIGUSR1);
        }
    }
    else if (signal == SIGTERM) {
        for (int i=0; i<N; ++i) {
            kill(children_pids[i], SIGTERM);
        }

        printf("[PARENT] Exitting parent process...\n");
        exit(1);
    }
}

void set_signal_action() {
    // Declare the sigaction structure
	struct sigaction act;

    // Set all of the structure's bits to 0 to avoid errors relating to uninitialized variables
	bzero(&act, sizeof(act));

    // Set the signal handler as the default action
	act.sa_handler = &sig_handler;

    // act.sa_flags = SA_RESETHAND; // SA_RESTORER

    // Apply actions to the signals
	sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
}



int main(int argc, char* argv[]) {
    set_signal_action();

    pid_t child = fork();
    pid_t pid = getpid();
    pid_t ppid = getppid();
    char child_procees[] = "./child";

    if (child < 0) {
        printf("Error: Couldn't create the child-process\n");
        return 1;
    }
    
    // Capturing Children
    int i = 0;
    children_pids[i] = child;
    ++i;

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
        
        // If the user want's to sent a signal, they could use the command: `kill -SIGNAM pid`
        // Which has three parts: command | signal type | pid

        N = 1;
        total_children = N;

        char command[10];
        char sig[20];
        pid_t sig_pid;

        while (1) {
            scanf("%s", command);

            if (strcmp(command, "kill")) {
                printf("The system can't undertand '%s'. You can only use 'kill' command. Nothing has been modified.\nUsage: kill -SIGNAL pid\n", command);
            }
            else {
                scanf("%s", sig);

                if (strcmp(sig, "-SIGTERM") && strcmp(sig, "-SIGUSR1")) {
                    printf("The system only understands 'SIGTERM', 'SIGUSR1', 'SIGUSR1'. Nothing has been modified.\n");
                }
                else {
                    scanf("%d", &sig_pid);

                    // Condition if the pid doesn't exists
                    int find = 0;
                    for (int i=0; i<N; ++i) {
                        if (children_pids[i] == sig_pid || sig_pid == pid) {
                            find = 1;
                            break;
                        }
                    }
                    if (!find) {
                        printf("The pid you've entered is not a child's, not the parent process. Nothing has been modified.\n");
                    }
                    else {
                        // Sending the Signals
                        if (!strcmp(sig, "-SIGTERM")) {
                            if (kill(sig_pid, SIGTERM) == -1) {
                                return 1;
                            }
                        }
                        else if (!strcmp(sig, "-SIGUSR1")) {
                            if (kill(sig_pid, SIGUSR1) == -1) {
                                return 1;
                            }
                        }
                    }
                }
            }

            // If a child has been terminated
            if (total_children < N) {
                printf("A child has been terminated or exited.\n");
                return 0;
            }

            // Clearing the input buffer
            while (getchar() != '\n');
        }
    }

    return 0;
}
