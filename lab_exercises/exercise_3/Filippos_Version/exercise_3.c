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



int main(int argc, char* argv[]){ //argc = number of arguments (1 default), argv = arguments

int N; //ammount of children parent will create
N=atoi(argv[1]); //argument to intiger function

for (int i=0; i<N; i++){
    if ((child=fork())==-1){
        perror("fork");
        exit(0);
    } else if (child==0){
        printf("Child %d PID: %d with parent id:%d\n", i, getpid(), getppid());
        exit(0);
    }else{
        wait(NULL);
    }
} 


}