#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <asm/signal.h>
#include <asm-generic/siginfo.h>
#define MAX_CHILDREN 1000000


struct sigaction {
void (*sa_handler)(int); // καθορίζει τη δράση που πρέπει να συσχετιστεί με το signum
void (*sa_sigaction)(int, siginfo_t *, void *); // Εάν ορίζεται SA_SIGINFO στο sa_flags, τότε
                                                 // στο sa_sigaction (αντί του sa_handler) καθορίζεται η λειτουργία χειρισμού για το signum
                                                // μην εκχωρήσετε τιμή και στα δύο: sa_handler και sa_sigaction
sigset_t sa_mask; //καθορίζει μια μάσκα σημάτων που πρέπει να αποκλειστούν
int sa_flags; //καθορίζει ένα σύνολο σημαιών που τροποποιούν τη συμπεριφορά του σήματος
void (*sa_restorer)(void); // Το πεδίο δεν προορίζεται για application use
};

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);


int main(int argc, char *argv[]){
    pid_t parent = getpid();
    pid_t signal_pid;
    pid_t child_pid[MAX_CHILDREN];
//check that command line argument is one and only one
int find =-1; //this is a helping variable to check if the pid exists in the array or is parent pid

 if (argc!=2){
        printf("Usage: ./gates Nstates\n");
        return 1;
 }
int N = strlen(argv[1]); //changed line because of segfault
 //check for --help argument
if (strcmp(argv[1], "--help") == 0) {
    printf("Usage: ./gates Nstates\n");
    return 1;
}

    //creating N children processes
    for (int i=0; i<N; i++){
        int status;
        pid_t child = fork();
        pid_t pid = getpid();
        pid_t ppid = getppid();

        if (child==-1){
            perror("Fork didn't work");
            return 1;
        }
        if (child==0){
            printf("[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'\n",ppid,i,pid,argv[1][i]);
            child_pid[i]=pid;
            char *args[]={"./child",i,NULL};
            execvp(args[0],args); //execvp replaces the current process with a new process
            perror("execvp failed\n");
            exit(EXIT_FAILURE); //this stops the child process from creating more children, termination of child process
        }
        else{
            if (wait(&status)==-1){
                printf("Error: Process has no child-proceses\n");
                return 1;
                }
        }
    }

char command[10];
char signal[10];
//check for input
while (1){
    scanf("%s",command);
    if (strcmp(command,"kill")){ //if true, this returns 0, if not it returns something different than 0
        printf("Wrong usage of command.\n Usage is 'Kill -Signal PID\n");
    }else{
        scanf("s",signal);
        if (!(strcmp(signal,"SIGTERM") || strcmp(signal,"SIGUSR1") || strcmp(signal,"SIGUSR2"))){
            printf("Wrong usage of command.\n Usage is 'Kill -Signal PID\n");
        }else{
            scanf("%d",&signal_pid);
            if (signal_pid==parent){
                find = parent;
            } else {
                for (int i=0; i<N; i++){
                    if (signal_pid==child_pid[i]){
                        find = child_pid[i];
                    } else { 
                        if (find==-1){
                            printf("PID not found\nNo changes have been made \n");
                        }
                    }
                }
            }
        }
    }
    //lets pass the signals to the children || parent | Here we have -command which we have to convert to signal
    
}



}