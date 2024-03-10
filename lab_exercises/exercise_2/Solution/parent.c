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

    The exec family of functions replaces the current running process with a new
    process. It can be used to run a C program by using another C program.
    All statements are ignored after execvp() call as this whole process(execDemo.c)
    is replaced by another process (EXEC.c).
        int execv(const char *path, char *const argv[]);
    - path: should point to the path of the file being executed. 
    - argv[]: is a null terminated array of character pointers.
*/



// Declearing them as global, because we need them inside `sig_handler`
pid_t child;
pid_t pid;
pid_t ppid;

int children_pids[100];
int N;
int wait_status;

int find;


// This function creates a child process and saving some pids for later use
int create_child(pid_t *child, pid_t *pid, pid_t *ppid, int index, char state) {
    *child = fork();
    *pid = getpid();
    *ppid = getppid();

    if (*child < 0) {
        printf("Error: Couldn't create the child-process\n");
        return 1;
    }

    // Capturing Children
    children_pids[index] = *child;

    char child_procees[] = "./child";
    if (*child == 0) {
        // Child's Code
        printf("[PARENT/PID=%d] Created child %d (PID=%d)\n", *ppid, index, *pid);

        char child_id[5];
        sprintf(child_id, "%d", index);

        char child_state = state;

        char *args[] = {child_procees, child_id, &child_state, NULL};
        if (execv(child_procees, args) == -1) {
            printf("Error: child.c file can't be executed.\n");
            return 1;
        }
    }
    return 0;
}


void sig_handler(int signal) {
    if (signal == SIGCHLD) {
        if (wait_status == 2304) {
            // Child's state was 't'
            if (create_child(&child, &pid, &ppid, find, 't') == 1) {
                exit(1);
            }
            if (child != 0) {
                printf("[PARENT/PID=%d] Created new child for gate %d (PID %d) and initial state 't'\n", pid, find, child);
            }
        }
        else if (wait_status == 2560) {
            // Child's state was 't'
            if (create_child(&child, &pid, &ppid, find, 'f') == 1) {
                exit(1);
            }
            if (child != 0) {
                printf("[PARENT/PID=%d] Created new child for gate %d (PID %d) and initial state 'f'\n", pid, find, child);
            }
        }
    }
    else if (signal == SIGUSR1) {
        for (int i=0; i<N; ++i) {
            kill(children_pids[i], SIGUSR1);
        }
    }
    else if (signal == SIGTERM) {
        for (int i=0; i<N; ++i) {
            wait_status = 0;
            printf("[PARENT/PID=%d] Waiting for %d children to exit\n", pid, N-i);
            kill(children_pids[i], SIGTERM);
            if (waitpid(children_pids[i], 0, 0) == -1) {
                exit(1);
            }
            printf("[PARENT/PID=%d] Child with PID=%d terminated successfully with exit status code 0!\n", pid, children_pids[i]);
        }

        printf("[PARENT/PID=%d] All children exited, terminating as well\n", pid);
        exit(0);
    }
    else if (signal == SIGUSR2) {
        for (int i=0; i<N; ++i) {
            kill(children_pids[i], SIGUSR2);
        }
    }
    else if (signal == SIGSTOP) {
        // Resuming the process by doing nothing when something trying to pause it.
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
	sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGSTOP, &act, NULL);
}


int correct_argument(char* arg) {
    char *temp = arg;
    int correct = 1;

    while (*temp != '\0') {
        if (*temp != 't' && *temp != 'f') {
            correct = 0;
            break;
        }
        ++temp;
    }
    return correct;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./gates fttfft\n");
        return 1;
    }
    if (!strcmp(argv[1], "--help")) {
        printf("Usage: ./gates fttfft\n");
        return 0;
    }
    if (!correct_argument(argv[1])) {
        printf("Argument must contain only 't', or 'f'.\nUsage: ./gates fttfft\n");
        return 1;
    }

    set_signal_action();
    
    N = strlen(argv[1]); // The number of children to create
    for (int i=0; i<N; ++i) {
        if (create_child(&child, &pid, &ppid, i, argv[1][i]) == -1) {
            return 1;
        }
    }
    
    if (child != 0) {
        // Parent's Code

        // If the user want's to sent a signal, they could use the command: `kill -SIGNAM pid`
        // Which has three parts: command | signal type | pid
        char command[10];
        char sig[20];
        pid_t sig_pid;

        while (1) {
            scanf("%s", command);

            // Check if the command is `kill`, otherwise clear the buffer and try again.
            if (strcmp(command, "kill")) {
                printf("The system can't undertand '%s'. You can only use 'kill' command. Nothing has been modified.\nUsage: kill -SIGNAL pid\n", command);
            }
            else {
                scanf("%s", sig);

                // Check if the command is SIGTERM, SIGUSR1, or SIGUSR2, otherwise clear the buffer and try again
                if (strcmp(sig, "-SIGTERM") && strcmp(sig, "-SIGUSR1") && strcmp(sig, "-SIGUSR2")) {
                    printf("The system only understands 'SIGTERM', 'SIGUSR1', 'SIGUSR2'. Nothing has been modified.\n");
                }
                else {
                    scanf("%d", &sig_pid);

                    // Find if the given pid is one of the children's.
                    find = -1;
                    for (int i=0; i<N; ++i) {
                        if (children_pids[i] == sig_pid || sig_pid == pid) {
                            find = i;
                            break;
                        }
                    }
                    if (find == -1) {
                        printf("The pid you've entered is not a child's, not the parent process. Nothing has been modified.\n");
                    }
                    else {
                        // The command is valid, so sending signals to the given process
                        if (!strcmp(sig, "-SIGTERM")) {
                            if (kill(sig_pid, SIGTERM) == -1) {
                                return 1;
                            }
                            printf("[PARENT/PID=%d] Child %d with PID=%d exited\n", pid, find, sig_pid);
                            if (sig_pid != pid) {
                                if (waitpid(sig_pid, &wait_status, 0) == -1) {
                                    return -1;
                                }
                            }
                        }
                        else if (!strcmp(sig, "-SIGUSR1")) {
                            if (kill(sig_pid, SIGUSR1) == -1) {
                                return 1;
                            }
                        }
                        else if (!strcmp(sig, "-SIGUSR2")) {
                            if (kill(sig_pid, SIGUSR2) == -1) {
                                return 1;
                            }
                        }
                    }
                }
            }

            // Clearing the input buffer
            while (getchar() != '\n');
        }
    }

    return 0;
}
