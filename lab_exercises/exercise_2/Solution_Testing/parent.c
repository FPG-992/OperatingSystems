#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char *argv[]){
    int N = strlen(argv[1]);
    pid_t parent = getpid();

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
        pid_t pid = getpid();
        pid_t ppid = getppid();
        pid_t child = fork();

        if (child==-1){
            perror("Fork didn't work");
            return 1;
        }
        if (child==0){
            printf("[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'\n",ppid,i,pid,argv[1][i]);
        }
        else{
            if (wait(&status)==-1){
                printf("Error: Process has no child-proceses\n");
                return 1;
                }
        }
    }

}