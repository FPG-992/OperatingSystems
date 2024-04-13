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
