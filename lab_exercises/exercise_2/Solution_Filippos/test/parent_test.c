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
pid_t child_pid[MAX_CHILDREN];

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

int N = strlen(argv[1]); // For creating child processes

}