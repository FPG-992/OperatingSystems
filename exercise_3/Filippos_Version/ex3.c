#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include<stdbool.h>
#include <errno.h>
#include<ctype.h>
#include <sys/select.h>
#include <time.h>
#define BUF_SIZE 64

void check_neg(int ret, const char *msg) 
{
  if (ret < 0) {
    perror(msg);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char **argv)
{
  bool round_robin = false;
  bool randomm = false;
  int number_of_processes ;
  int *p_id;
  char buf[BUF_SIZE];

  if (argc == 2)
  {
    for(int i=0; i<strlen(argv[1]); i++)
    {
        if (!isdigit(argv[1][i]))
        {
            printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
            exit(1);
        }
    }
    round_robin = true;
  }
   else if (argc == 3)
    {
        for(int i=0; i<strlen(argv[1]); i++)
        {
          if (!isdigit(argv[1][i]))
          {
            printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
            exit(1);
          }
        }

        if(strcmp(argv[2],"--round-robin") == 0){
            round_robin = true;
        }
        else if(strcmp(argv[2],"--random") == 0){
            randomm = true;
        }
        else{
            printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
            exit(1);
        }
    }
    else
    {
      printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
      exit(1);
    }

   number_of_processes = atoi(argv[1]);

   if (number_of_processes <= 0) 
    {
        printf("Invalid number of children: %s\n", argv[1]);
        exit(1);
    }

   p_id = (int *)malloc(number_of_processes * sizeof(int)); //Allocate memory for p_id array
   if (p_id == NULL) 
    {
      printf("Memory allocation failed.\n");
      return 1;
    }

   int **pipe_fd_parent_to_child; //Create array for pipeline, the sender is the parent and receiver the child in this array
   pipe_fd_parent_to_child = (int **)malloc(number_of_processes * sizeof(int *));  //Allocate memory for p_id array pointers
   for (int i = 0; i < number_of_processes; i++) 
    {
        pipe_fd_parent_to_child[i] = (int *)malloc(2 * sizeof(int));  //For each line we allocate memory for 2 columns
        if (pipe_fd_parent_to_child[i] == NULL) 
        { //We check if the allocation is correct
            printf("Memory allocation failed");
            exit(1);
        }
    }

    int **pipe_fd_child_to_parent;//Create array for pipeline, the sender is the child and receiver the parent in this array
    pipe_fd_child_to_parent = (int **)malloc(number_of_processes * sizeof(int *));  //Allocate memory for p_id array pointers
    for (int i = 0; i < number_of_processes; i++) 
    {
        pipe_fd_child_to_parent[i] = (int *)malloc(2 * sizeof(int));//For each line we allocate memory for 2 columns
        if (pipe_fd_child_to_parent[i] == NULL) 
        {   //We check if the allocation is correct
            printf("Memory allocation failed");
            exit(1);
        }
    }

  for (int i = 0; i < number_of_processes; i++)
    {
        if (pipe(pipe_fd_parent_to_child[i]) == -1) {           //Here we create the pipes and check for possible errors
            perror("pipe");
            exit(1);
        }
    }

for (int i = 0; i < number_of_processes; i++)
      {
        if (pipe(pipe_fd_child_to_parent[i]) == -1) {           //Here we create the pipes and check for possible errors
            perror("pipe");
            exit(1);
        }
      }


  for (int j = 0; j < number_of_processes; j++)
  {
        pid_t p = fork();
        check_neg(p, "fork");
        if (p == 0)
        {
            int val;  //Parameter that comes from the parent and the child must increase and send back
            close(pipe_fd_child_to_parent[j][0]); //As the sender the child doesnt read
            close(pipe_fd_parent_to_child[j][1]); //As the receiver the child doesnt send
            while(1)                              //This loop never ends until a sigterm is received
            {
                if(read(pipe_fd_parent_to_child[j][0], &val, sizeof(int))==-1)
                    printf("Error reading from parent\n");
                else
                    printf("[Child %d] [%d] Child received %d!\n", j, getpid(), val);
                val++;
                sleep(5);
                if(write(pipe_fd_child_to_parent[j][1], &val, sizeof(int))==-1)
                    printf("Error sending to parent from child %d\n",j);
                else
                    printf("[Child %d] [%d] Child Finished hard work, writing back %d\n", j, getpid(), val);
            }
        }
        else if (p > 0)
        {
        p_id[j] = p;
        }
        else
            perror("error");
    }
    int job;                  //The number that the parent must sent to its children
    fd_set read_set;          //Here we define read_set in which we will add all the possible coming input either from terminal or from the children's pipelines
    int child_iteration = 0;  //Which child must receive the job

    for(int i = 0; i<number_of_processes; i++)
    {
      close(pipe_fd_child_to_parent[i][1]);//As the receiver the parent doesnt write to child
      close(pipe_fd_parent_to_child[i][0]);//As the sender the parent doest read from child
    }
     
   while (1)
   {
        int ndfs = 0;                                    //This is the first argument of select(), maximum file descriptor value
        FD_ZERO(&read_set);                              //We initialize in each repetition which object is ready to read
        FD_SET(0,&read_set);                             //We load the terminal input (value 0) into our ready-to-read set//πηγη πιθανου ινπουτ
        for (int i = 0; i < number_of_processes; i++)    //In this for loop we add all the read ends of the children pipelines
        {                                                //and we assign the highest value of all file descriptors to ndfs (terminal has 0 value) 
            FD_SET(pipe_fd_child_to_parent[i][0], &read_set);
            if (pipe_fd_child_to_parent[i][0] > ndfs) 
            {
                ndfs = pipe_fd_child_to_parent[i][0];          //ndfs should be the highest-numbered file descriptor
            }
        }

        int retval = select(ndfs + 1, &read_set, NULL, NULL, NULL); //We only care to read some input, so the three last select variables are NULL (write,except,timeout)
        if ( retval < 0) 
        {
            perror("select");
            return 1;
        }


        if (FD_ISSET(0, &read_set))
        {
            fgets(buf,BUF_SIZE,stdin);//We receive input data from terminal
            bool is_integer = false;
            if (strcmp(buf,"exit\n") == 0)
            {
                //terminates the children processes
                int number = number_of_processes;
                int status;
                for (int i = 0; i < number_of_processes; i++)
                {
                    if(i==number_of_processes-1)
                        printf("Waiting for %d child to exit\n",number);
                    else
                        printf("Waiting for %d children to exit\n",number);
                    kill(p_id[i],SIGTERM);
                    --number;
                    if(wait(&status) == -1) //Waiting for the child to terminate
                    {
                        printf("There are no child processes to wait for\n");
                    }
                }
                printf("All children terminated\n");
                exit(0);
            }
            else if (strcmp(buf,"help\n") == 0)
            {
                printf("Type a number to send a job to a child\n");
            }
            else  //We check if it is a positive integer with is_integer
            {
                char *endptr;
                long num = strtol(buf,&endptr,10);
                if (endptr == buf || *endptr != '\n' || errno == ERANGE)
                {
                    printf("Type a number to send a job to a child\n");
                }
                else
                {
                    job = atoi(buf);
                    if(round_robin)
                    {
                        if(child_iteration == number_of_processes)
                        {
                            child_iteration = 0;
                        }        
                        printf("[Parent] Assigned job %d to child %d\n", job, child_iteration);
                        if(write(pipe_fd_parent_to_child[child_iteration][1], &job, sizeof(int))==-1)
                            printf("Error sending to child %d", child_iteration);
                        child_iteration++;
                    }

                    if(randomm)
                    {
                        srand(time(NULL));
                        child_iteration = rand()%number_of_processes;      //Randomly selecting a child with rand, rand mod number to generate numbers from 0 to number
                        printf("[Parent] Assigned job %d to child %d\n", job, child_iteration);
                        if(write(pipe_fd_parent_to_child[child_iteration][1], &job, sizeof(int))==-1)
                            printf("Error sending to child %d", child_iteration);    //Its up to the programmer if he wants to exit if an error occurs
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < number_of_processes; i++) 
            {
                if (FD_ISSET(pipe_fd_child_to_parent[i][0], &read_set))     //We iterate over the file descriptors of the child channel
                {
                    if(read(pipe_fd_child_to_parent[i][0], &job, sizeof(int)) == -1)
                        printf("Error reading from child\n");
                    else
                        printf("[Parent] Value received from child %d: %d\n", i, job);
                }  
            }
        }
    }
 return 0;
}
