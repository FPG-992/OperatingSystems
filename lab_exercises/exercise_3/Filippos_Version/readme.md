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


int ret = poll(fds, N+1, timeout); //polling for input
        int ret = poll(fds, N+1, timeout); //polling for input
        if (ret == -1){
            perror("poll");
            exit(EXIT_FAILURE);
        }

        int timeout = -1; //timeout for poll function (-1 = infinite)