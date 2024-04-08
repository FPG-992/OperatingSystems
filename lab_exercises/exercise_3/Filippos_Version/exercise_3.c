#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <poll.h>




int main(int argc, char* argv[]){ //argc = number of arguments (1 default), argv = arguments
int pd[2]; //pipe descriptor , one edge for reading and one for writing| PD0 = Read Edge | PD1=Write Edge
pipe(pd);

printf("ARGV: %s\n", argv[1]);

printf("%d\n", pd[0]);
printf("%d\n", pd[1]);



}