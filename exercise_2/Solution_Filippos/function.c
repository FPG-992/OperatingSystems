/* function for executing the commands
if (strcmp(signal,"-SIGTERM")==0){
                if (signal_pid==parent){
                    for(int i=0; i<N; i++){
                        kill(child_pid[i],SIGTERM);
                        printf("Child %d terminated\n",child_pid[i]);
                    }
                } else {
                    kill(signal_pid,SIGTERM);
                    printf("Child %d terminated\n",signal_pid);
                }
            }
            if (strcmp(signal,"-SIGUSR1")==0){
                if (signal_pid==parent){
                    for(int i=0; i<N; i++){
                        kill(child_pid[i],SIGUSR1);
                        printf("Child %d received SIGUSR1\n",child_pid[i]);
                    }
                } else {
                    kill(signal_pid,SIGUSR1);
                    printf("Child %d received SIGUSR1\n",signal_pid);
                }
            }
            if (strcmp(signal,"-SIGUSR2")==0){
                if (signal_pid==parent){
                    for(int i=0; i<N; i++){
                        kill(child_pid[i],SIGUSR2);
                        printf("Child %d received SIGUSR2\n",child_pid[i]);
                    }
                } else {
                    kill(signal_pid,SIGUSR2);
                    printf("Child %d received SIGUSR2\n",signal_pid);
                }
            }
*/