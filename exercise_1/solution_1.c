#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>

bool file_exists(const char *filename){ //exw ena bool gia na elegxw an to arxeio yparxei
    struct stat buffer; //to struct stat einai ena struct pou periexei plirofories gia to arxeio
    return stat(filename,&buffer)==0 ? true : false; //to stat epistrefei 0 an to arxeio yparxei ternary conditional operator
}
int main(int argc, char *argv[]){
    const char *filename = argv[1];
//dieukrinisi 1,2,3
if (argc <2 || argc > 2){ //ligotero apo 1 argument kai perissotero apo 1 argument, default to argc einai 1
    printf("Usage: ./a.out filename");
    return 1; 
    } else if (file_exists(filename)==true){ //an to arxeio yparxei
        printf("Error: %s already exists\n",argv[1]);
        return 1;
    } else if (strcmp(argv[1], "--help") == 0) {
    printf("Usage: ./a.out filename\n");
    return 0;
}
//dieukrinish 3

    int fd = open(argv[1], O_CREAT | O_APPEND | O_WRONLY, 0644); //auto edw einai gia thn eggrafh sto arxeio 
    
    int status;
    pid_t child;
    child = fork();

    char buf[100]; //dinw arketes theseis gia na mhn exw provlima me to buffer 
    //sthn ekfwnish ths ergasthriakhs uphrxe paradeigma to opoio omws de mporei na efarmostei
    //ston sugkekrimeno kwdika


    if (child<0){
        perror("dhmiourgia pediou apetixe");
    }   else if (child==0){
        pid_t parent_pid;
        parent_pid = getppid();
        printf("[CHILD] getpid()= %d, getppid()=%d\n",getpid(),getppid());
        
        sprintf(buf, "[CHILD] getpid()= %d, getppid()=%d\n",getpid(),getppid()); //sprintf gia na grapsei to periexomeno sto buf (storeprintf)
        if (write(fd, buf, strlen(buf)) < strlen(buf)){
            perror("write");
            return 1;
        }
        close(fd);

        exit(0);
    } else { //kwdikas tou patera
        wait(&status);
        printf("[PARENT] getpid()= %d, getppid()=%d\n",getpid(),getppid());

        if (fd == -1){
            perror("open");
            return 1;
        }

        sprintf(buf, "[PARENT] getpid()= %d, getppid()=%d\n",getpid(),getppid()); //sprintf gia na grapsei to periexomeno sto buf (storeprintf)
        if (write(fd, buf, strlen(buf)) < strlen(buf)){
            perror("write");
            return 1;
        }
        close(fd);
        
        exit(0); 
            }

    return 0;
}