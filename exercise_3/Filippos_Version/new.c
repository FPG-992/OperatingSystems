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

#define READ_END 0  //read end of pipe
#define WRITE_END 1 //write end of pipe 

//---- is_digit function ----

int is_Digit(char *str){ //function to check if a string is a digit
    for (int i=0; i<strlen(str); i++){
        if (!isdigit(str[i])){
            return 0;
        }
    }    return 1;
}

//---Global Variables---

pid_t pid;

int main (int argc, char* argv[]){

int N = atoi(argv[1]); //argument to intiger function

char command [100]; //command buffer to read input

int childpids[N]; //array to store the child pids

int task; //the number given to the child to process to decrement by the terminal

int child_id=0; //child id

int task_to; //children which has gotten the task

//-------start to check for Command Line arguments-------
int default_mode = 0; //default mode = round robin (0) or random (1)
if (argc==2 && N>=1){
    printf("Default mode: Round Robin \n");
}else if (argc==3 && N>=1 && (strcmp(argv[2],"--round-robin")==0)){
    printf("Default mode: Round Robin\n");
}else if (argc==3 && N>=1 && (strcmp(argv[2],"--random")==0)){
    default_mode = 1;
    printf("Default mode: Random\n");
}else{
    printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
    return 0;
}

//----Create pipes-----
int parent_to_child[N][2]; //pipe parent to child
int child_to_parent[N][2]; //receive pipe descriptor

//----Initialize pipes-----
for (int i=0; i<N; i++){
    if(pipe(parent_to_child[i])==-1){
        perror("pipe");
        return 1;
    }
    if(pipe(child_to_parent[i])==-1){
        perror("pipe");
        return 1;
    }
}


//---- Create N Children -----
for (int i=0; i<N; i++){
    childpids[i] = fork(); //forking the children
    if (childpids[i] == -1){ //if fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (childpids[i] == 0){ //if fork succeeded
    printf("Child %d created with PID: %d\n",i,getpid());
        break;
    }
}

//--- Set up poll structure for terminal parent and children ----

struct pollfd TERMINAL;
TERMINAL.fd = 0; //stdin
TERMINAL.events = POLLIN; //polling for input

struct pollfd CHILD[N];
struct pollfd PARENT[N];

for (int i=0; i<N; i++){
    CHILD[i].fd = child_to_parent[i][READ_END];
    CHILD[i].events = POLLIN;

    PARENT[i].fd = parent_to_child[i][READ_END];
    PARENT[i].events = POLLIN;
}

//---Iterating for every child to execute it's code----
    for (int i=0; i<N; ++i) {
        if (childpids[i] == 0) {
            // Child's code

            // Closing the connection we don't need
            
            close(parent_to_child[i][WRITE_END]);
            close(child_to_parent[i][READ_END]);

            while (1) {
                // Wait untill the parent has succesfully sent a message to the child i
                if (poll(&PARENT[i].fd, 1, 0) == -1) {
                    perror("poll");
                }
                if (PARENT[i].revents & POLLIN) {
                    read(parent_to_child[i][READ_END], &task, sizeof(task));
                    printf("[Child=%d, pid=%d] Received number: %d\n", i, getpid(), task);

                    // Wait for 10 seconds
                    sleep(1);

                    // Decrement the number
                    task--;
                    printf("[Child=%d, pid=%d] Decremented number: %d\n", i, getpid(), task);
                    // Send message to the parent
                    if(write(child_to_parent[i][WRITE_END], &task, sizeof(task))==-1){
                        perror("write");
                        exit(EXIT_FAILURE);
                        } else {printf("Task sent to parent\n");
                        int test;
                        read(child_to_parent[i][READ_END], &test, sizeof(test));
                        printf("test gives: %d\n",test);
                        }
                }
            }
        }
    }


//-----------------Parent Process-----------------

while (1){
    close(parent_to_child[task_to][READ_END]);
    close(child_to_parent[task_to][WRITE_END]);
    
        scanf("%s",command); //read the command
        printf("Command:%s\n",command); //print the command
        if(strcmp(command,"exit")==0){ //if the command is exit
            for (int i=0; i<N; i++){
                kill(childpids[i],SIGTERM); //send SIGTERM to all children
                printf("Child %d pid: %d terminated\n",i,childpids[i]);
            }
            //terminate all children processes & parent
            printf("Parent pid: %d terminated\n",getpid());
            exit(0);
        }else if (strcmp(command,"help")==0){
            printf("Type a number to send job to a child!\n");
        }else if (is_Digit(command)==1){ //check if it is an integer , then distribute number to child //CODING BLOCK OF IS DIGIT
            task = atoi(command);  
            if(default_mode==0){ //round-robin
            child_id = (child_id + 1) % N; 
            task_to = child_id; 
            }
            if(default_mode==1){ //random
            child_id = rand() % (N); 
            task_to = child_id;
            }
            printf("[Parent, pid=%d] Assigned %d to child %d (pid=%d)\n", getpid(), task, child_id, childpids[task_to]);
            //write task to child process
            if(write(parent_to_child[task_to][WRITE_END], &task, sizeof(task))==-1){
                perror("write");
                exit(EXIT_FAILURE);
            } else {
                int debug;
                read(parent_to_child[task_to][READ_END], &debug, sizeof(debug));
                printf("WRITE SUCCESFULL | Task sent to child %d with PID:%d\n",task_to,childpids[task_to]);
                printf("read gives: %d\n",debug);
            }
        } //CODING BLOCK OF IS DIGIT
        else {
            printf("Type a number to send job to a child!\n");
        }
}





// Closing all the connections
    for (int i=0; i<N; ++i) {
        close(PARENT[i].fd);
        close(CHILD[i].fd);
    }



return 0;
}