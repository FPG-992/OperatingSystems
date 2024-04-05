#define _XOPEN_SOURCE 700 // resolves the `struct sigaction sa` incomplete type error

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>


void sig_handler(int signal) {
    if (signal == SIGTERM) {
        printf("[Child] Terminating child (pid=%d)\n", getpid());
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

    // Apply the actions to the signals
    sigaction(SIGTERM, &act, NULL);
}



int main(int argc, char* argv[]) {
    set_signal_action();

    printf("[Child] Started execution.\n");
    printf("[Child] PID: %d, Index: %s\n", getpid(), argv[1]);

    while (1) {
        continue;
    }

    return 0;
}
