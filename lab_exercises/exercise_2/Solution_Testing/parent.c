#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

//alarm set up
unsigned int alarm(unsigned int seconds);

//signal handler
void signal_handler(int signum){}

int main(int argc, char *argv[]){
    int N = strlen(argv[1]);
    pid_t parent = getpid();

while(1){
//set alarm for 15 seconds
alarm(15);
//signal setup
signal(SIGALRM, signal_handler);
}

//check that command line argument is one and only one
 if (argc!=2){
        printf("Usage: ./gates Nstates\n");
        return 1;
 }

 //check for --help argument
if (strcmp(argv[1], "--help") == 0) {
    printf("Usage: ./gates Nstates\n");
    return 1;
}
//PJF is checking if the line argument contains something other than t or f 


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
            exit(0); //this stops the child process from creating more children
        }
        else{
            if (wait(&status)==-1){
                printf("Error: Process has no child-proceses\n");
                return 1;
                }
        }
    }

}