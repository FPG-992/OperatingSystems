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
    else if (signal == SIGUSR1) {
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

    // Apply the actions to the signals
	sigaction(SIGALRM, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
}


int main() {
    time(&t); // capturing in the variable 't' the system seconds

	set_signal_action();

    /*
    Tο bug που βρηκα ειναι οτι αν κληθουν στο καπακι δυο alarm, μονο το τελευταιο θα λιετουργησει
    Για τον λογο αυτο δεν χρησιμοποιω την alarm, διοτι το παρακατω timer δεν εχει μεγαλη ακριβεια
    με αποτελεσμα να συνεπιπταν εντος του χρονου, δυο η περισσοτερα alarms, οποτε εχανα μερικα 
    print states.
    */

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
