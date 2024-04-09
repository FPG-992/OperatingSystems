#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <poll.h>
pid_t pid; //process id global variable
pid_t child; //child process global variable

int parent_to_child[2]; //pipe parent to child
int child_to_parent[2]; //receive pipe descriptor   


int main(int argc, char* argv[]){ //argc = number of arguments (1 default), argv = arguments

int N; //ammount of children parent will create
N=atoi(argv[1]); //argument to intiger function

//start to check for Command Line arguments
int default_mode = 0; //default mode = round robin (0) or random (1)
if (argc==2 && N>=1){
    printf("Default mode: Round Robin\n");
}else if (argc==3 && N>=1 && (strcmp(argv[2],"--round-robin")==0)){
    printf("Default mode: Round Robin\n");
}else if (argc==3 && N>=1 && (strcmp(argv[2],"--random")==0)){
    default_mode = 1;
    printf("Default mode: Random\n");
}else{
    printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
    return 0;
}


if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1){ //create pipes and check for errors simultaneously
    perror("pipe");
    exit(0);
}

for (int i=0; i<N; i++){
    if ((child=fork())==-1){
        perror("fork");
        exit(0);
    } else if (child==0){
        printf("Child %d PID: %d with parent id:%d\n", i, getpid(), getppid());
        exit(0);
    }else{ //parent
        wait(NULL);
    }
} 


}