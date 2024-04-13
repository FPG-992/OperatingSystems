Series of tasks in Code:

Check for arguments = Done
Array for pipe descriptors = Done 
Poll Structure = Done
Create Pipes for each children = Yes
  Pass poll parameters to each poll object = Not done
Create Children = Done
For loop for children to execute task = Not Done
For loop for parent code to capture children 
  Closing Connections
  Sending message
For loop for checking if a child has sent a message to parent 
For loop for closing all connections
Parent process to receive commands from terminal = On Going

for (int i=0; i<N; i++){
            if (fds[i+1].revents & POLLIN){
                int message;
                if (read(child_to_parent[i][READ_END], &message, sizeof(message))==-1){
                    perror("read");
                    exit(EXIT_FAILURE);
                }else{
                    printf("[Parent, pid=%d] Received number: %d from child %d (pid=%d)\n", pid, message, i, childpids[i]);
                    //send message back to child
                    if (write(parent_to_child[i][WRITE_END], &message, sizeof(message))==-1){
                        perror("write");
                        exit(EXIT_FAILURE);
                    }else{
                        printf("Message sent back to child %d\n",i);
                    }
                }
            }
        }



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