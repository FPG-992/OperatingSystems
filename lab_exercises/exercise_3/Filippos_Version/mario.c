/**
 * \file mario.c
 * \author Aggelos Kolaitis <neoaggelos@gmail.com>
 * \date 2024-03-24
 * \brief Example with pipes and input selection
 *
 * This program demonstrates the usage of poll() for reading from multiple
 * file descriptors without blocking on either one.
 *
 * The initial process creates two unnamed pipes, and forks a child process. The
 * child process simply writes a random number on a pipe, at random. The father
 * process checks for input from both pipes.
 */

#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define WHITE "\033[37m"
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(int argc, char **argv) {
  // Create pipes, before fork!
  int pd[2][2];
  for (int i = 0; i < 2; i++) {
    if (pipe(pd[i]) != 0) {
      perror("pipe");
    }
  }

  // Initialize random seed
  srand((unsigned int)time(NULL));

  // Fork child
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(1);
  }

  if (pid == 0) {
    int value = 1000;

    // close read fds, we will not use them.
    close(pd[0][0]);
    close(pd[1][0]);

    for (int i = 0; i < 10; i++) {
      // pick index of pipe to write to
      int idx = rand() % 2;

      printf("Write to pipe %d\n", idx);

      // write one integer to the pipe every 5 seconds
      write(pd[idx][1], &value, sizeof(int)); // error checking!
      sleep(5);
    }

    // close pipe write fd and exit()
    close(pd[0][1]);
    close(pd[1][1]);

    // children exit here, so the rest of the code is for the parent
    exit(0);
  }

  // Parent code

  // Create an array of pollfds for the two pipes.
  struct pollfd *pollfds = malloc(2 * sizeof(struct pollfd));

  // We will poll (.fd = pd[0][0]) for reading (.events = POLLIN)
  pollfds[0].fd = pd[0][0];
  pollfds[0].events = POLLIN;

  // We will poll (.fd = pd[1][0]) for reading (.events = POLLIN)
  pollfds[1].fd = pd[1][0];
  pollfds[1].events = POLLIN;

  while (1) {
    // poll() will block until any of the fds are ready to read() from
    if (poll(pollfds, 2, -1) < 0) {
      perror("poll");
      continue; // this is not a good idea, but just try again
    }

    // the kernel will set (.revents) with the ready events. check for each pipe
    // if POLLIN is part of the bitmask.
    for (int i = 0; i < 2; i++) {
      if (pollfds[i].revents & POLLIN) {
        int val;
        read(pollfds[i].fd, &val, sizeof(int)); // error checking!
        printf(MAGENTA "Got input from pipe %d: '%d'" WHITE "\n", i, val);
      }
    }
  }
}
