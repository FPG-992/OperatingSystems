  // Iterating for every child to execute it's code
    for (int i=0; i<N; ++i) {
        if (child_pid[i] == 0) {
            // Child's code

            // Closing the connection we don't need
            close(fds[i][p2c][WRITEEND]);
            close(fds[i][c2p][READEND]);

            while (1) {
                // Wait untill the parent has succesfully sent a message to the child i
                if (poll(p2c_pfds[i], 1, 0) == -1) {
                    errExit("poll");
                }
                if (p2c_pfds[i][0].revents & POLLIN) {
                    read(fds[i][p2c][READEND], c_read_buffer[i], sizeof(c_read_buffer[i]));
                    printf("[Child=%d, pid=%d] Received number: %s\n", i, getpid(), c_read_buffer[i]);

                    // Wait for 10 seconds
                    sleep(10);

                    // Decrement the number
                    number = string_to_int(c_read_buffer[i]) - 1;

                    // Send message to the parent
                    strcpy(message[i], int_to_string(number));
                    write(fds[i][c2p][WRITEEND], message[i], (strlen(message[i]) + 1));
                }
            }
        }
    }

    //polu zoumi h upothesi edw petro