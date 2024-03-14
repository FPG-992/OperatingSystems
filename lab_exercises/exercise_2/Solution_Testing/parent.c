#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#define MAX_CHILDREN 1000000
int main(int argc, char *argv[]){
    pid_t parent = getpid();
    pid_t signal_pid;
    pid_t child_pid[MAX_CHILDREN];
//check that command line argument is one and only one
 if (argc!=2){
        printf("Usage: ./gates Nstates\n");
        return 1;
 }
int N = strlen(argv[1]); //changed line because of segfault
 //check for --help argument
if (strcmp(argv[1], "--help") == 0) {
    printf("Usage: ./gates Nstates\n");
    return 1;
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
            exit(0); //this stops the child process from creating more children
        }
        else{
            if (wait(&status)==-1){
                printf("Error: Process has no child-proceses\n");
                return 1;
                }
        }
    }

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
            for (int i=0;i<N; i++){
                if (signal_pid=parent){
                break;
                }
                if (child_pid[i]==signal_pid){
                    kill(signal_pid,signal);
                    printf("Sent signal %s to process %d\n",signal,signal_pid);
                }
            }
        }
    }
}



}