#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <Kernel/string.h>



int main(int argc, char *argv[]){
    int N = strlen(argv[1]);
    pid_t parent = getpid();

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
            printf()
        }
    }



}