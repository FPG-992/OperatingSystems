#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>


/*
    Εργαλεια

    The exec family of functions replaces the current running process with a new
    process. It can be used to run a C program by using another C program.
    All statements are ignored after execvp() call as this whole process(execDemo.c)
    is replaced by another process (EXEC.c).
        int execv(const char *path, char *const argv[]);
    - path: should point to the path of the file being executed. 
    - argv[]: is a null terminated array of character pointers.

*/



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

    int N = strlen(argv[1]);              // The number of childer to create
    char child_procees[] = "./child";     // The full path of the child-process executable

    // Creating the N child processes
    for (int i=0; i<N; ++i) {
        int status;
        pid_t child = fork();
        pid_t pid = getpid();
        pid_t ppid = getppid();

        if (child < 0) {
            printf("Error: Couldn't create the child-process\n");
            return 1;
        }
        if (child == 0) {
            // Child Code
            printf("[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'\n", ppid, i, pid, argv[1][i]);

            char str_id[50];
            sprintf(str_id, "%d", i);

            char *args[] = {child_procees, str_id, NULL}; // The arguments of the 'execv' sys call

            if (execv(child_procees, args) == -1) {
                printf("Error: child.c file can't be executed.\n");
                return 1;
            }
        }
        else {
            // Parent Code
            if (wait(&status) == -1) {
                printf("Error: Process has no child-proceses\n");
                return 1;
            }
        }
    }

    return 0;
}
