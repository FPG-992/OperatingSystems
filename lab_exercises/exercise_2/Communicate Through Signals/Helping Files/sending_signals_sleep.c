#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/*
    Εργαλεια
    
    The parent process sends signals to the child process using the kill() function,
    passing the child process ID and the signal number as arguments.
    When a signal is received, the corresponding signal handler function is called.
    We need a way for the child process to keep running and waiting for signals to
    be received. That's why we need to create an infinite loop.

    kill() - The kill() function signals the process or a group of processes. The
    process (or group of processes) to which the signal is to be sent is specified
    by the pid. On successful completion of kill(), 0 is returned; otherwise, -1 is
    returned.
    
    signal() - The child process picks up the signals with signal() and calls appropriate
    functions.
*/



void sighup(int arg) {
    signal(SIGHUP, sighup); /* reset signal */
    printf("CHILD: I have received a SIGHUP\n");
}
void sigint(int arg) {
    signal(SIGINT, sigint); /* reset signal */
    printf("CHILD: I have received a SIGINT\n");
}
void sigquit(int arg) {
    printf("My DADDY has Killed me!!!\n");
    exit(0);
}

 
int main() {
    pid_t child_pid;
 
    /* get child process */
    if ((child_pid = fork()) < 0) {
        perror("fork");
        return 1;
    }
 
    if (child_pid == 0) {
        // Child's Code
        signal(SIGHUP, sighup);
        signal(SIGINT, sigint);
        signal(SIGQUIT, sigquit);
        for (;;)
            ; /* loop for ever */
    }
    else {
        // Parent's Code
        printf("\nPARENT: sending SIGHUP\n\n");
        kill(child_pid, SIGHUP);
 
        sleep(3); /* pause for 3 secs */
        printf("\nPARENT: sending SIGINT\n\n");
        kill(child_pid, SIGINT);
 
        sleep(3); /* pause for 3 secs */
        printf("\nPARENT: sending SIGQUIT\n\n");
        kill(child_pid, SIGQUIT);
        sleep(3);
    }
}
