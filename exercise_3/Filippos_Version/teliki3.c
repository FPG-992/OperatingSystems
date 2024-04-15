#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>
#include <sys/select.h>  // include the whole family

void usage(const char *prog) {
    printf("Usage: %s <nChildren> [--random] [--round-robin]\n",prog);
    exit(EXIT_FAILURE);
}

 //-----------------Function to check if input is integer--------------------////
bool isNumeric( char str[]) {
   for (int i = 0; i < strlen(str); i++)
      if (isdigit(str[i]) == false)
      return false; //when one non numeric value is found, return false   
   return true;
}
//-----------------Function to check if 3rd argument is valid job assignment method---------------//////////
#define STREQUAL(x, y) ( strncmp((x), (y), strlen(y) ) == 0 )
#define MAX(a, b) ((a) > (b) ? (a) : (b))


int main(int argc, char** argv) {
///-----------------------------------------CHECKING ARGUMENTS--------------------//////////
	int how_many;
	int method=0;  //increment method of children job is 0 for round robin, 1 for random
     for (int i = 1; i < argc; i++) {
        printf("Argument %d: '%s'\n", i, argv[i]);
    }
    	if ( (argc>3) || (argc<2) )  
    		usage(argv[0]);
  
        if (!(isNumeric(argv[1])) )         //if not integer,exit
            usage(argv[0]);
        else
        {
         how_many=atoi(argv[1]);
        if (how_many<=0)                           //if negative or zero exit
        {
            usage(argv[0]);         
        }
                
        }

        if (argc==3)
        {
        	if (! ( STREQUAL(argv[2],"--round-robin") || STREQUAL(argv[2],"--random") ) )
        	{
        		usage(argv[0]);
        	}
        	else
        	{
        		method= STREQUAL(argv[2],"--random");
        	}

        }

 //-------------------END OF CHECKING ARGUMENTS---------------------////////////////   

int readpd[how_many][2];    //child reads from father ([0]),so father writes to child ([1])
int writepd[how_many][2];   //child writes to father ([1]),so father reads from the child ([0])
int dogtags[how_many];      //pids of children to end their lives D: 
                        /////============Create the pipelines=============================/////
     
     for (int q = 0; q < how_many; ++q)
    {
        pipe(&readpd[q][0]); // [0] read port, [1] write port, matrix for n children reads.
        if (pipe(&readpd[q][0])!= 0) 
        {
        perror("pipe");
        exit(EXIT_FAILURE);
        }
        pipe(&writepd[q][0]);// [0] read port, [1] write port, matrix for n children writes. (children give info)
        if (pipe(&writepd[q][0])!= 0)
         {
        perror("pipe");
        exit(EXIT_FAILURE);
         }
    }
                            //////////////------------------------------Create the children------------///////////////////

     for (int i = 0; i <how_many; ++i)
    {
        pid_t pid=fork();

        if (pid==-1)
        {
            perror("fork");
            exit(1);
        }

        if (pid==0)  // CHILD process  
        {
            int val;
            
            close(writepd[i][0]); // we don't need child to read in its write pipeline
            close(readpd[i][1]);  //we don't need child to write in its read pipeline
            

            while(1) 
            {


            int fd_input=read(readpd[i][0], &val, sizeof(int)); 

            if (fd_input == -1) 
                    {
                        perror("read");
                        exit(-1);
                    }

            printf("[Child %d]: [%d] Child received %d!""\n", i, getpid(), val);
            val++;
            sleep(5);

            int fd_output=write(writepd[i][1], &val, sizeof(int));// pou ta grafw,apo pou 
            if (fd_output == -1) 
                    {
                        perror("write");
                        exit(-1);
                    }
            printf("[Child %d]: [%d] Child Finished hard work, writing back %d.\n", i, getpid(), val);
            
            }
        

        }
                  dogtags[i]=pid; //technically inside father code --mazeuw ta pid ton paidiwn 


        }  //end for loop
        
/////////////////////////////---------------FATHER CODE----------------------------------/////////////
//Tora thelw o pateras na diavazei apo STDIN ti arithmo dwsame kai apo ta write[howmany][0] to val kai na grafei sto readpd[howmany][1] ton arithmo pou dwsame 
            for (int l=0; l<how_many; l++)   //close pipeline ports that won't be needed
        {
            close(readpd[l][0]); //kleinw ta read ports twn paidiwn 
            close(writepd[l][1]); //kleinw ta write ports twn paidiwn


        }
        int counter=0;


            while (1) {
        fd_set inset;
        int maxfd;

        FD_ZERO(&inset);                // we must initialize before each call to select
        FD_SET(STDIN_FILENO, &inset);   // select will check for input from stdin
        for (int k=0; k<how_many; k++)
        {
             FD_SET(writepd[k][0], &inset);          // select will check for input from pipe

        }
        maxfd=STDIN_FILENO;
        // select only considers file descriptors that are smaller than maxfd
        for (int j=0; j<how_many; j++)
        {
            maxfd = MAX(maxfd,writepd[j][0]);

        }
        maxfd = maxfd + 1;


        // wait until any of the input file descriptors are ready to receive
        int ready_fds = select(maxfd, &inset, NULL, NULL, NULL);
        if (ready_fds <= 0) {
            perror("select");
            continue;                                       // just try again
        }

        // user has typed something, we can read from stdin without blocking
        if (FD_ISSET(STDIN_FILENO, &inset)) {
            char buffer[101];
            int n_read = read(STDIN_FILENO, buffer, 100);   // error checking!
        if (n_read == -1) 
                {
                    perror("read");
                    exit(-1);
                }

            buffer[n_read] = '\0';                          // why?

            // New-line is also read from the stream, discard it.
            if (n_read > 0 && buffer[n_read-1] == '\n') {
                buffer[n_read-1] = '\0';
            }
            


            if (n_read >= 4 && strncmp(buffer, "exit", 4) == 0) {
                // user typed 'exit', kill child and exit properly

                for (int rip = 0; rip < how_many; ++rip)
                 /* code */
            {
            int w = waitpid(dogtags[rip], NULL, WNOHANG);
            if (w==0)
            {
                printf("[Father process: %d] Will terminate (SIGTERM) child process %d: %d\n",getpid(),rip,dogtags[rip] );
                kill(dogtags[rip],SIGTERM);
               wait(NULL);

            }
/*NEWWW*/   close(writepd[rip][0]); //father also closes the port from where he read from the children 
            close(readpd[rip][1]); //father also closes the ports from where he wrote to his chilfren 
            }                    
                exit(0);
            }
            
            else if (n_read >= 1 && isNumeric(buffer))  // BE CAREFUL, IF WE SET N_READ>=4 ,numbers will not be recognised 
                                                        //because '2' is recognised as char aka 1 byte.
            {

                int x=atoi(buffer);
                int whoyou=counter%how_many;

                if (method==0)
                {


                    int Wbytes=write(readpd[whoyou][1], &x, sizeof(int));

                if (Wbytes == -1) 
                {
                    perror("write");
                    exit(-1);
                }

                 printf("[Parent] Assigned %d to child %d.\n", x,whoyou);

            counter++;
                 }
                 else if (method==1)
                 {
                    int tuxaios=rand()%how_many;
                    int Wbytes=write(readpd[tuxaios][1],&x,sizeof(int));

                if (Wbytes == -1) 
                {
                    perror("write");
                    exit(-1);
                }

                 printf("[Parent] Assigned %d to child %d.\n", x,tuxaios);





                 }

            
            }

            else if (n_read >= 1)   //if user types in Help or gibberish, makes no difference 
            {
                printf("Type a number to send job to a child!\n"); 
            }
        }

        // someone has written bytes to the pipe, we can read without blocking
        for (int m = 0; m < how_many; ++m)
        {

        
            if (FD_ISSET(writepd[m][0], &inset)) 
            {
                int diavasma;
                int Rbytes=read(writepd[m][0], &diavasma, sizeof(int));                 // error checking!
                if (Rbytes == -1) 
                {
                    perror("read");
                    exit(-1);
                }
             printf("[Parent] Received result form child %d ----> %d.\n", m,diavasma);


            }

        }
         




                         } //end while




    return 0;
}
// Authors:
// Maria Moutousi
// Dimitris Lampros