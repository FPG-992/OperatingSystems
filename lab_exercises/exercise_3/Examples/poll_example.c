#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>


int main() {
    int pd[2];

    pipe(pd);

    // Setting the poll structure
    struct pollfd pfds[2];
    pfds[0].fd = pd[0];
    pfds[1].fd = pd[1];

    // Setting events
    pfds[0].events = POLLIN;
    pfds[1].events = POLLOUT;

    poll(pfds, 2, 0);

    char b[10] = "Hi";
    char bb[10];

    write(pd[1], b, strlen(b));

    pfds[0].revents = POLLIN;

    read(pd[0], bb, strlen(b));

    sleep(2);

    printf("%s\n", bb);

    // The file is ready to be read.
    if (pfds[0].revents == POLLIN) {
        printf("The read-end can accept read operations.\n");
    }
    else {
        printf("The read-end cannot accept read operations.\n" );
    }

    // The file is ready to be write.
    if (pfds[1].revents == POLLOUT)
        printf("The write-end can accept write operations.\n" );
    else {
        printf("The write-end cannot accept write operations.\n" );
    }

    // POLLERR: An error occurred.
    // POLLHUP: In socket or pipes, indicates that the other end closed the connection

    close (pfds[0].fd);
    close (pfds[1].fd);

    return 0;
}

