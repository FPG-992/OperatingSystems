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

pid_t pid; //process id global variable
pid_t child; //child process global variable

int is_Digit(char *str){ //function to check if a string is a digit
    for (int i=0; i<strlen(str); i++){
        if (!isdigit(str[i])){
            return 0;
        }
    }
    return 1;
}


int main(int argc, char* argv[]){ //argc = number of arguments (1 default), argv = arguments
char command [100]; //command buffer to read input

int task; //the number given to the child to process to decrement
int N; //ammount of children parent will create
N=atoi(argv[1]); //argument to intiger function

int childpids[N]; //array to store the child pids
int child_id; //shoutout to pjf for the idea
int task_to; //children which has gotten the task

//dynamic allocation for pipes
int (*parent_to_child)[2] = malloc(N*sizeof(*parent_to_child));    //pipe parent to child
int (*child_to_parent)[2] = malloc(N*sizeof(*child_to_parent));   //receive pipe descriptor  

//create poll structure
struct pollfd fds[N+1]; //N child_to_parent pipes + 1 for stdin (CLI)
//for child to parent pipes
for (int i=0; i<N; i++){
    fds[i].fd = child_to_parent[i][0]; //read end of pipe
    fds[i].events = POLLIN; //check for data to read
} 
//for stdin
fds[N].fd = STDIN_FILENO; //stdin
fds[N].events = POLLIN; //check for data to read

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

for (int i=0; i<N; i++){
    if ((child=fork())==-1){
        perror("fork");
        exit(0);
    } else if (child==0){
        close(parent_to_child[i][1]); //close write end of pipe since this is for parent to write and child only reads
        close(child_to_parent[i][0]); //close read end of pipe since child only writes and parent doesn't read here
        printf("Child %d PID: %d with parent id:%d\n", i, getpid(), getppid());
        
        //entering infinite loop for child to receive tasks execute and send back to parent

        while(1){
            ssize_t bytes_read = read(parent_to_child[i][0], &task, sizeof(task)); //the size read from parent, the number
            if (bytes_read <= 0){ //we use this to check if the read failed 
                break;
            }else {childpids[i] = getpid();}
            printf("Child %d received task: %d and is now processing\n", i, task); //for debugging purposes
            task--; //decrement the number by 1 
            sleep(10); //sleep for 10 seconds as asked from the exercise , this is the processing time, avoiding using <time.h>
            printf("Child %d finished processing task: %d\n", i, task); //for debugging purposes
            write(child_to_parent[i][1], &task, sizeof(task)); //send the number back to parent using the write end of child_to_parent
        }
        close(parent_to_child[i][0]); //not used anymore
        close(child_to_parent[i][1]); //not used anymore

        exit(0);
    }else{ //parent 
    close(parent_to_child[i][0]); //close read end of pipe of parent to child since the child is the one who reads
    close(child_to_parent[i][1]); //close write end of pipe of child to parent since the child is not writing here
    int quit = 0; //quit flag
    while (!quit){
        int pollcount = poll(fds, N+1, -1); //poll for data to read
        if (pollcount<0){
            perror("poll failed"); //if error than exit
            exit(EXIT_FAILURE);
        }
        if (fds[N].revents & POLLIN){ //if there is data to read from stdin
        fgets(command,sizeof(command),stdin); //read the command
        }
        if(strcmp(command,"exit")==0){ //if the command is exit
            quit = 1; //set the quit flag to 1
            //terminate all children processes & parent

            break; //break the loop
        }else if (strcmp(command,"help")==0){
            printf("Type a number to send job to a child!\n");
        }else if (is_Digit(command)>0){ //check if it is an integer , then distribute number to child 
            task = atoi(command);      //to decrement by 1 and send back to parent using random or round-robin | convert command to int 
            if(default_mode==0){ //round-robin
            child_id = (child_id + 1) % N; 
            task_to = child_id;
            }
            if(default_mode==1){ //random
            child_id = rand() % (N + 1); 
            task_to = child_id;
            }
        }else {
            printf("Type a number to send job to a child!\n");
        }
    }


    close(parent_to_child[i][1]);
    close(child_to_parent[i][0]);
    }
} 



for (int i=0; i<N; i++){ //for each child
    wait(NULL);
}

free(parent_to_child); //free memory of pipe
free(child_to_parent); //free memory of pipe

return 0;
}