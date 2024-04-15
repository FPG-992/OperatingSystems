for (int i=0; i<N; i++){

    if(childpids[i]==0){ //this is child's code
    printf("This is a child process with PID: %d\n",getpid());
        close(child_to_parent[i][READ_END]); //close read end of pipe
        close (parent_to_child[i][WRITE_END]); //close write end of pipe

        while(1){
                if (read(parent_to_child[i][READ_END], &task, sizeof(task))==-1){
                    perror("read");
                    exit(EXIT_FAILURE);
                }else{
                    printf("READ SUCCESFULL | Task received from parent: %d\n",task);
                }
            }

        }

    }