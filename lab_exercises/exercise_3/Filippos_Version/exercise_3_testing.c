#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <poll.h>
#include <ctype.h>
#include <signal.h>

#define READ_END 0
#define WRITE_END 1
//read end is 0
//write end is 1

int test = 404;


pid_t pid; //process id global variable
pid_t child; //child process global variable

int is_Digit(char *str){ //function to check if a string is a digit
    for (int i=0; i<strlen(str); i++){
        if (!isdigit(str[i])){
            return 0;
        }
    }    return 1;
}

int main(int argc, char* argv[]){ //argc = number of arguments (1 default), argv = arguments
char command [100]; //command buffer to read input

int task; //the number given to the child to process to decrement
int N; //ammount of children parent will create
N=atoi(argv[1]); //argument to intiger function

int childpids[N]; //array to store the child pids
int child_id; //shoutout to pjf for the idea
int task_to; //children which has gotten the task

//pipe initialization
int (parent_to_child)[N][2]; //pipe parent to child
int (child_to_parent)[N][2]; //receive pipe descriptor  

//structure for poll function
struct pollfd fds[N+1]; //array of pollfd structures
fds[0].fd = 0; //fds[0] is the stdin
fds[0].events = POLLIN; //polling for input
//initialize the pollfd structure for the children
for (int i=0; i<N; i++){
    fds[i+1].fd = child_to_parent[i][READ_END]; //read end of pipe
    fds[i+1].events = POLLIN; //polling for input
}


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


for (int i=0; i<N; i++){ //create pipes for n proccesses
if (pipe(parent_to_child[i]) == -1 || pipe(child_to_parent[i]) == -1){ //create pipes and check for errors simultaneously
    perror("PIPE CREATION FAILED");
    exit(EXIT_FAILURE); }
}

//creating children
for (int i=0; i<N; i++){
    if ((child=fork())==-1){
        perror("FORK FAILED");
        exit(EXIT_FAILURE);
    }else if (child==0){
        //child code
        //printing created child i with pid
        printf("Child %d created with pid: %d and ppid: %d\n",i,getpid(),getppid());
        exit(0);
    }else {
        wait(NULL);
        //parent code
        childpids[i] = child; //store the child pids
        //print child pids
        printf("Child %d pid: %d\n",i,childpids[i]);
    }
}


/*debugging purposes
for (int i=0; i<N; i++){
    printf("Child %d pid: %d\n",i,childpids[i]);
}
debugging purposes */

//parent process to receive commands from terminal
pid = getpid(); //get parent pid

int quit = 0; //quit flag

int timeout = -1; //timeout for poll function -1 means infinite
    while (!quit){
        int ret = poll(fds, N+1, timeout); //this value is the number of file descriptors that have events
        if (ret == -1){
            perror("poll");
            exit(EXIT_FAILURE);
        }
        if (fds[0].revents & POLLIN){ //if there is input from stdin
        scanf("%s",command); //read the command
        printf("Command:%s\n",command); //print the command
        if(strcmp(command,"exit")==0){ //if the command is exit
            quit = 1; //set the quit flag to 1
            for (int i=0; i<N; i++){
                kill(childpids[i],SIGTERM); //send SIGTERM to all children
                printf("Child %d pid: %d terminated\n",i,childpids[i]);
            }
            //terminate all children processes & parent
            exit(0);
        }else if (strcmp(command,"help")==0){
            printf("Type a number to send job to a child!\n");
        }else if (is_Digit(command)==1){ //check if it is an integer , then distribute number to child 
            task = atoi(command);  
            if(default_mode==0){ //round-robin
            child_id = (child_id + 1) % N; 
            task_to = child_id; 
            }
            if(default_mode==1){ //random
            child_id = rand() % (N + 1); 
            task_to = child_id;
            }
            printf("[Parent, pid=%d] Assigned %d to child %d (pid=%d)\n", pid, task, child_id, childpids[task_to]);
            //close unused pipes before sending task to child process
            //close(parent_to_child[task_to][READ_END]); this causes the parent to hang
            close(child_to_parent[task_to][WRITE_END]); //close write end of pipe
            //write task to child process
            if(write(parent_to_child[task_to][WRITE_END], &task, sizeof(task))==-1){
                perror("write");
                exit(EXIT_FAILURE);
            } else { printf("Task sent to child %d\n",task_to);}
        }else {
            printf("Type a number to send job to a child!\n");
        }

        for (int i=0; i<N; i++){
            if (fds[i+1].revents & POLLIN){
                int number; // the number the child has sent to the parent
                if (read(child_to_parent[i][READ_END], &number, sizeof(number))==-1){
                    perror("read");
                    exit(EXIT_FAILURE);
                } else {
                    printf("[Parent, pid=%d] Received number: %d from child %d (pid=%d)\n", pid, number, i, childpids[i]);
                    // Sending back message to the child i
                    if(write(parent_to_child[i][WRITE_END], &number, sizeof(number))==-1){ //parent sends the number back to the child to decrement again
                        perror("write");
                        exit(EXIT_FAILURE);
                    } else { printf("Parent sent number %d back to child %d with pid\n",number,i,child_id[i]); }
                }
            }
        }
        //now check for messages from children

    }


return 0;
}