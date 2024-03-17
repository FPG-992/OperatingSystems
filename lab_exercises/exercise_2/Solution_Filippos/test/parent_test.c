#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MAX_CHILDREN 1000000

int main(int argc, char* argv[]){
pid_t parent = getpid();
pid_t signal_pid;
pid_t child_pid[MAX_CHILDREN]; //array to store child pids
char command[10];
char signal[10];

if (argv[1] != NULL && strcmp(argv[1], "--help") == 0) { //error exception because !=NULL was not in the if statement
    printf("Usage: ./gates Nstates\n");
    return 1;
}

if (argc!=2){ //check that command line argument is one and only one
    printf("Usage: ./gates t*f*\n");
    return 1;
}

for (int i=0; i<strlen(argv[1]); i++){ //check if there are only t and f in the command line argument
    if(argv[1][i]=='t' || argv[1][i]=='f'){
        continue;
    } else {
        printf("Usage: ./gates t*f*\n");
        return 1;}
        }

//now it is needed to check for input
while(1){
    scanf("%s",command);
    if (strcmp(command,"kill")!=0){ //if true, this returns 0, if not it returns something different than 0
        continue;
    }else {
        printf("Wrong usage of command.\n Usage is 'kill -Signal PID\n");
    }
}








//creating N children processes
int N = strlen(argv[1]); // For creating child processes

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
        char str[10]; //buffer to store the integer value of i
        sprintf(str, "%d", i); //converts i to string
        //char *args[]={"./child",i,NULL};
        // execvp(args[0],args); //execvp replaces the current process with a new process
        // perror("execvp failed\n");
        exit(EXIT_FAILURE); //this stops the child process from creating more children, termination of child process
    }
    else{ //parent process
        if (wait(&status)==-1){
            printf("Error: Process has no child-proceses\n");
            return 1;
            }
    }
}


}