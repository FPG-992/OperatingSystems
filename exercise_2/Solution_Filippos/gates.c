#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MAX_CHILDREN 1000000
#define _XOPEN_SOURCE 700 // resolves the `struct sigaction sa` incomplete type error

pid_t parent;
pid_t child;
pid_t signal_pid;
pid_t child_pid[MAX_CHILDREN]; //array to store child pids
pid_t pid;
pid_t ppid;
int child_index;
char command[10];
char signal_str[10];
pid_t find;
char idx[10]; //buffer to store the integer value of i
int N;
int wait_status;

int create_child(pid_t *child, pid_t *pid, pid_t *ppid, int index, char state) {
    *child = fork();
    *pid = getpid();
    *ppid = getppid();

    if (*child < 0) {
        printf("Error: Couldn't create the child-process\n");
        return 1;
    }

    // Capturing Children
    child_pid[index] = *child;

    char child_procees[] = "./child";
    if (*child == 0) {
        // Child's Code
        printf("[PARENT/PID=%d] Created child %d (PID=%d)\n", *ppid, index, *pid);

        char child_id[5];
        sprintf(child_id, "%d", index);

        char child_state = state;

        char *args[] = {child_procees, child_id, &child_state, NULL};
        if (execv(child_procees, args) == -1) {
            printf("Error: child.c file can't be executed.\n");
            return 1;
        }
    }
    return 0;
}

void handler(int signal){
    if (signal==SIGCHLD){
        if (wait_status==2304){
            //child state was 't'
            if (create_child(&child, &child_pid[child_index], &parent, child_index, 't') == -1){
                exit(1);
            }
            if (child!=0){
             printf("[PARENT/PID=%d] Created new child for gate %d (PID %d) and initial state 't'\n", parent, child_index, child);   
            }
        }
    }else if (wait_status == 2560) {
            // Child's state was 't'
            if (create_child(&child, &pid, &ppid, child_index, 'f') == 1) {
                exit(1);
            }
            if (child != 0) {
                printf("[PARENT/PID=%d] Created new child for gate %d (PID %d) and initial state 'f'\n", pid, find, child);
            }
    }
  if (signal==SIGTERM){
                if (signal_pid==parent){
                    for(int i=0; i<N; i++){
                        kill(child_pid[i],SIGTERM);
                        printf("Child %d terminated\n",child_pid[i]);
                    }
                } else {
                    kill(signal_pid,SIGTERM);
                    printf("Child %d terminated\n",signal_pid);
                }
            }
            if (signal==SIGUSR1){
                if (signal_pid==parent){
                    for(int i=0; i<N; i++){
                        kill(child_pid[i],SIGUSR1);
                        printf("Child %d received SIGUSR1\n",child_pid[i]);
                    }
                } else {
                    kill(signal_pid,SIGUSR1);
                    printf("Child %d received SIGUSR1\n",signal_pid);
                }
            }
            if (signal==SIGUSR2){
                if (signal_pid==parent){
                    for(int i=0; i<N; i++){
                        kill(child_pid[i],SIGUSR2);
                        printf("Child %d received SIGUSR2\n",child_pid[i]);
                    }
                } else {
                    kill(signal_pid,SIGUSR2);
                    printf("Child %d received SIGUSR2\n",signal_pid);
                }
            }  
}

void setup_sigaction() {
    struct sigaction act; // Declare a struct to hold signal action settings
    bzero(&act, sizeof(act)); // Initialize all members of the struct to zero
    act.sa_handler = handler; // Set the handler function to be our defined function
    //apply actions to the signals
    sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGSTOP, &act, NULL);
}


int main(int argc, char* argv[]){
parent=getpid();
//enabling sigaction
setup_sigaction();
//check for argc, argv input errors
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
        return 1;
        }
}

//creating N children processes
N = strlen(argv[1]); // For creating child processes

for (int i=0; i<N; i++){
    if(create_child(&child, &child_pid[i], &parent, i, argv[1][i]) == -1){
        return 1;
    }
}

//checking for command input
if (child!=0){ // parent's code
while(1){
        scanf("%s %s %d",command,signal_str,&signal_pid);
        printf("Command: %s, Signal: %s, PID: %d\n",command,signal_str,signal_pid);
        if (strcmp(command,"kill")!=0){ //if true, this returns 0, if not it returns something different than 0
            printf("Wrong usage of command.\nUsage is 'kill -Signal PID\n");
        }else {
            printf("Command is correct\n");
        }
        if (strcmp(signal_str,"-SIGTERM")!=0 && strcmp(signal_str,"-SIGUSR1")!=0 && strcmp(signal_str,"-SIGUSR2")!=0){
                printf("Wrong usage of Signal.\nUsage is 'Kill -Signal PID\n");
        }else {
            printf("Signal is correct\n");
        }
        find = -1; //find is set equal each time we get a new signal_pid
        if (signal_pid==parent){    
         find = parent;
         printf("Parent found\n");
        } else if(signal_pid!=parent){
            for (int i=0; i<N; i++){
                if (signal_pid==child_pid[i]){
                    find = child_pid[i];
                    child_index = i;
                    printf("Child found\n");
                    break;
                } 
            }
            if (find==-1){
                printf("PID not found\nNo changes have been made \n");
            }
        }
        if (find==-1){
            break;
        }else{
            if (strcmp(signal_str,"-SIGTERM")==0){
                if (kill(signal_pid,SIGTERM)==-1){
                    perror("Signal not sent\n");
                } 
                printf("[PARENT/PID=%d] Child with PID=%d exited\n", parent, child);
                if (signal_pid != parent) {
                    if (waitpid(signal_pid, &wait_status, 0) == -1) {
                        return -1;
                        }
            }
            if (strcmp(signal_str,"-SIGUSR1")==0){
                if (kill(signal_pid,SIGUSR1)==-1){
                    perror("Signal not sent\n");
                }
            }
            if (strcmp(signal_str,"-SIGUSR2")==0){
                if (kill(signal_pid,SIGUSR2)==-1){
                    perror("Signal not sent\n");
                }
            }
        }
}
           
    //clearing input buffer, Credits:PJF
    while (getchar() != '\n');
    
}
}

}