#include <stdio.h>
#include <poll.h>


int main() {
    struct pollfd fds[1];
    int ret;

    // Monitor standard input for input events
    fds[0].fd = 0; // File descriptor for standard input
    fds[0].events = POLLIN; // Monitor for input events

    // Wait for events on the monitored file descriptors
    ret = poll(fds, 1, 5000); // Wait for up to 5 seconds

    if (ret > 0) {
        // Events occurred
        if (fds[0].revents & POLLIN) {
            printf("Data is available on standard input.\n");
        }
    } else if (ret == 0) {
        // Timeout occurred
        printf("Timeout occurred.\n");
    } else {
        // Error occurred
        perror("poll");
        return 1;
    }

    return 0;
}
