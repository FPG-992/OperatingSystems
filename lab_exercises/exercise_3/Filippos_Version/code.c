while (1){
        scanf("%s",command); //read the command
        printf("Command:%s\n",command); //print the command
        if(strcmp(command,"exit")==0){ //if the command is exit
            for (int i=0; i<N; i++){
                kill(childpids[i],SIGTERM); //send SIGTERM to all children
                printf("Child %d pid: %d terminated\n",i,childpids[i]);
            }
            //terminate all children processes & parent
            printf("Parent pid: %d terminated\n",getpid());
            exit(0);
        }else if (strcmp(command,"help")==0){
            printf("Type a number to send job to a child!\n");
        }else if (is_Digit(command)==1){ //check if it is an integer , then distribute number to child 
            task = atoi(command);  
            if(default_mode==0){ //round-robin
            child_id = (child_id + 1) % N; 
            task_to = child_id; 
            }
            if(default_mode==1){ //random
            child_id = rand() % (N); 
            task_to = child_id;
            }
            printf("[Parent, pid=%d] Assigned %d to child %d (pid=%d)\n", pid, task, child_id, childpids[task_to]);
            //write task to child process
            if(write(parent_to_child[task_to][WRITE_END], &task, sizeof(task))==-1){
                perror("write");
                exit(EXIT_FAILURE);
            } else {
                int debug;
                read(parent_to_child[task_to][READ_END], &debug, sizeof(debug));
                printf("WRITE SUCCESFULL | Task sent to child %d with PID:%d\n",task_to,childpids[task_to]);
                printf("read gives: %d\n",debug);
            }
        }else {
            printf("Type a number to send job to a child!\n");
        }
        