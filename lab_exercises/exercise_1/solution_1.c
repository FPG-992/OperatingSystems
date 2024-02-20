#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
    int status;
    pid_t child;
    child = fork();

    if (child<0){
        perror("dhmiourgia pediou apetixe");
    }   else if (child==0){
        pid_t parent_pid;
        parent_pid = getppid();
        printf("[CHILD] getpid()= %d, getppid()=%d\n",getpid(),getppid());
        exit(0);
    } else { //kwdikas tou patera
        printf("[PARENT] getpid()= %d, getppid()=%d\n",getpid(),getppid());
        wait(&status);
        exit(0); 
            }
}