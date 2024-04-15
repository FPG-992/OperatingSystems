#define _XOPEN_SOURCE 700 // resolves the `struct sigaction sa` incomplete type error

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <strings.h>


time_t t;


void sig_handler(int signal) {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    time_t local_t;

    time(&local_t);

	if (signal == SIGALRM) {
        printf("[CHILD] getpid() = %d, getppid() = %d | time: %ld\n", pid, ppid, local_t - t);
    }
    else if (signal == SIGTERM) {
        printf("[Child] Terminating proceess child pid: %d\n", pid);
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

    // Apply the action in the structure to the SIGALRM signal
	sigaction(SIGALRM, &act, NULL);
}


int main() {
    // Printing the information for time=0, because alarm(0) doesn't set an alarm
    pid_t pid = getpid();
    pid_t ppid = getppid();
    printf("[CHILD] getpid() = %d, getppid() = %d | time: 0\n", pid, ppid);

    time(&t); // capturing in the variable 't' the system seconds

	set_signal_action();
    
	while(1) {
        alarm(5);
        sleep(5);
    };
	return 0;
}
