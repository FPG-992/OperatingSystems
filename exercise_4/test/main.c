#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <bits/getopt_core.h>


#define default_host "os4.iot.dslab.ds.open-cloud.xyz"
#define default_port 20241

void print_help(){
       printf("Available Commands:\n");
       printf("help - print this help message\n");
       printf("exit - exit the program\n");
       printf("get: retrieve server data");
       printf("N name surname reason : Request exit permission during quarantine\n");
}

int main(int argc, char *argv[]){
       char *host = default_host;
       int port = default_port;
       int debug = 0;
       int opt;

       while((opt = getopt(argc, argv, "host:port:debug")) != -1){
              switch(opt){
                     case 'host':
                            host = optarg;
                            break;
                     case 'port':
                            port = atoi(optarg);
                            break;
                     case 'debug':
                            debug = 1;
                            break;
                     default:
                            fprintf(stderr, "Usage: %s [-h host] [-p port] [-d]\n", argv[0]);
                            exit(EXIT_FAILURE);
              }
       }

       //creating socket
       int sockfd = socket(AF_INET, SOCK_STREAM, 0);
       if(sockfd < 0){
              perror("socket");
              exit(EXIT_FAILURE);
       }

       struct sockaddr_in server_addr;
       memset(&server_addr, 0, sizeof(server_addr));
       server_addr.sin_family = AF_INET;
       server_addr.sin_port = htons(port);
       server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

       bind(sockfd, &server_addr, sizeof(server_addr));

       //converting host to IP
       if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0){
              perror("inet_pton");
              exit(EXIT_FAILURE);
       }

       //connecting to server
       if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
              perror("connect");
              exit(EXIT_FAILURE);
       }

       printf("Connected to %s:%d\n", host, port);

}