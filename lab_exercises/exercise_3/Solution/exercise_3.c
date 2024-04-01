#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>




int main(int argc, char* argv[]){
int pd[2]; //pipe descriptor , one edge for reading and one for writing| PD1 = Write Edge | PD2=Read Edge
pipe(pd);






}