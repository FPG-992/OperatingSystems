This is a file for good written functions.

This is for save purposes

char command[10];
char signal[10];
//check for input
while (1){
    scanf("%s",command);
    if (strcmp(command,"kill")){ //if true, this returns 0, if not it returns something different than 0
        printf("Wrong usage of command.\n Usage is 'Kill -Signal PID\n");
    }else{
        scanf("s",signal);
        if (!(strcmp(signal,"SIGTERM") || strcmp(signal,"SIGUSR1") || strcmp(signal,"SIGUSR2"))){
            printf("Wrong usage of command.\n Usage is 'Kill -Signal PID\n");
        }else{
            scanf("%d",&signal_pid);
            if (signal_pid==parent){
                find = parent;
            } else {
                for (int i=0; i<N; i++){
                    if (signal_pid==child_pid[i]){
                        find = child_pid[i];
                    } else { 
                        if (find==-1){
                            printf("PID not found\nNo changes have been made \n");
                        }
                    }
                }
            }
        }
    }
    //lets pass the signals to the children || parent | Here we have -command which we have to convert to signal
    
}



//creating N children processes
    for (int i=0; i<N; i++){
        int status;
        pid_t child = fork();
        pid_t pid = getpid();
        pid_t ppid = getppid();

        if (child==-1){
            perror("Fork didn't work");
            return 1;
        }
        if (child==0){
            printf("[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'\n",ppid,i,pid,argv[1][i]);
            child_pid[i]=pid;
            char *args[]={"./child",i,NULL};
            execvp(args[0],args); //execvp replaces the current process with a new process
            perror("execvp failed\n");
            exit(EXIT_FAILURE); //this stops the child process from creating more children, termination of child process
        }
        else{
            if (wait(&status)==-1){
                printf("Error: Process has no child-proceses\n");
                return 1;
                }
        }
    }



    