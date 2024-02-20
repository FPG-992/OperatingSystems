#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]){
    int status;
    pid_t child;
    child = fork();

    const char *buf = "Test\n"; //to periexomeno pou tha grapsei sto arxeio
    

    if (child<0){
        perror("dhmiourgia pediou apetixe");
    }   else if (child==0){
        pid_t parent_pid;
        parent_pid = getppid();
        printf("[CHILD] getpid()= %d, getppid()=%d\n",getpid(),getppid());
        exit(0);
    } else { //kwdikas tou patera
        wait(&status);
        printf("[PARENT] getpid()= %d, getppid()=%d\n",getpid(),getppid());

        int fd = open(argv[1], O_CREAT | O_APPEND | O_WRONLY, 0644); //auto edw einai gia thn eggrafh sto arxeio 
        if (fd == -1){ //prepei na einai mesa sto parent process giati mono tote tha grapsw sto arxeio
            perror("open");
            return 1;
        }
        if (write(fd, buf, strlen(buf)) < strlen(buf)){
            perror("write");
            return 1;
        }
        close(fd);
        
        exit(0); 
            }

        
    

    return 0;
}