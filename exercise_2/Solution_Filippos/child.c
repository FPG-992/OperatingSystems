#define _XOPEN_SOURCE 700 // resolves the `struct sigaction sa` incomplete type error

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>



// Declearing them as global, because we need them inside `sig_handler`
time_t t;
char state;
char id[5];


void sig_handler(int signal) {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    time_t local_t;

    time(&local_t);

	if (signal == SIGALRM || signal == SIGUSR1) {
        if (state == 't') {
            printf("[ID=%s / PID=%d / TIME=%ld] The gates are open!\n", id, pid, local_t - t);
        }
        else {
            printf("[ID=%s / PID=%d / TIME=%ld] The gates are close!\n", id, pid, local_t - t);
        }
    }
    else if (signal == SIGTERM) {
        if (state == 't') {
            exit(9); // Returning the status of the state: t to the parent process
        }
        exit(10); // Returning the status of the state: f to the parrent process
    }
    else if (signal == SIGUSR2) {
        if (state == 't') {
            state = 'f';
            printf("[ID=%s / PID=%d / TIME=%ld] The gates are close!\n", id, pid, local_t - t);
        }
        else {
            state = 't';
            printf("[ID=%s / PID=%d / TIME=%ld] The gates are open!\n", id, pid, local_t - t);
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

    // Apply the actions to the signals
	sigaction(SIGALRM, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGSTOP, &act, NULL);
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("You must pass the id and the state of the child.\nUsage: ./child 0 t\n");
        return 1;
    }

    strcpy(id, argv[1]);
    state = *argv[2];

    time(&t); // capturing the system seconds in the variable t

	set_signal_action();

    /*
    Tο bug που βρηκα ειναι οτι αν κληθουν στο καπακι δυο alarm, μονο το τελευταιο θα λιετουργησει
    Για τον λογο αυτο δεν χρησιμοποιω την alarm, διοτι το παρακατω timer δεν εχει μεγαλη ακριβεια
    με αποτελεσμα να συμπιμπτουν δυο alarm εντος του χρονου (15 δευτερολεπτα), οποτε εχανα μερικα 
    print states.
    */

    // Create a timer and send `SIGALRM` signals manualy
    time_t current = time(0);
    time_t stop = current;
	while(1) {
        if (current >= stop) {
            kill(getpid(), SIGALRM);
            stop = current + 15;
        }
        current = time(0);
    }

	return 0;
}
