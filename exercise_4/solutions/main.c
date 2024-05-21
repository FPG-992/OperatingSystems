#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>
#include <netdb.h>


#define DEFAULT_HOST "os4.iot.dslab.ds.open-cloud.xyz"
#define DEFAULT_PORT 20241
#define BUFFER_SIZE 1024 // Buffer size for reading from socket, sending to socket, and reading from stdin

int debug = 0;

void print_help(){ //prints the help message
    printf("Available commands:\n");
    printf("help : Print this help message\n");
    printf("exit : Exit the program\n");
    printf("get : Retrieve data from the server\n");
    printf("N name surname reason : Request exit permission\n");
}

void parse_arguments(int argc, char *argv[], char**host, int *port){ //parses the arguments
    *host = DEFAULT_HOST;
    *port = DEFAULT_PORT;
    for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--host") == 0 && i + 1 < argc) {
                *host = argv[i + 1];
                i++;
            } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
                *port = atoi(argv[i + 1]);
                i++;
            } else if (strcmp(argv[i], "--debug") == 0) {
                debug = 1;
            } else {
                fprintf(stderr, "Unknown argument: %s\n", argv[i]);
                exit(EXIT_FAILURE);
            }
        }
}


int create_connect_socket(char *host, int port) { //creates a socket and connects to the server
    int sockfd; // Socket file descriptor
    struct sockaddr_in server_addr; // Server address
    struct hostent *hostp; // Host entry for resolving hostname to IP address

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { //socket is an endpoint for communication which | 2 IDS, port & host
        perror("socket creation failed"); //socket stream , tcp , reliable , OS checks for errors & packets
        exit(EXIT_FAILURE); //af_inet is the adress family for ipv4
    }

    // Resolve hostname to IP address because can't connect to just a name :)
    hostp = gethostbyname(host); 
    if (hostp == NULL) {
        fprintf(stderr, "Error: Invalid hostname\n");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure to connect to
    memset(&server_addr, 0, sizeof(server_addr)); // Zero out the structure
    server_addr.sin_family = AF_INET; // Internet address family
    server_addr.sin_port = htons(port); // Server port htons() converts host byte order to network byte order
    bcopy((char *)hostp->h_addr_list[0], (char *)&server_addr.sin_addr.s_addr, hostp->h_length); // Server IP address

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {  // Connect to the server
        perror("Connection Failed"); 
        close(sockfd);
        exit(EXIT_FAILURE);
    } else {
        printf("Connected to server Succesfully %s:%d\n", host, port); // Connection successful | Debugging
    }

    return sockfd;
}

void handle_get(int sockfd){ //handles the get command
    char buffer[BUFFER_SIZE]; // Buffer for sending and receiving data
    int n; 
    
    strcpy(buffer, "get\n"); //copy string into buffer
    send(sockfd, buffer, strlen(buffer), 0); // Send 'get' command to server

    if (debug) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
        printf("[DEBUG] sent 'get'\n"); // Debugging
    }

    n = recv(sockfd, buffer, BUFFER_SIZE, 0); // Receive data from server
    buffer[n] = '\0'; 

    if (debug) {
        buffer[strcspn(buffer, "\n")] = '\0'; //mark end of string
        printf("[DEBUG] received '%s'\n", buffer);
    }

    int event, light, temperature, timestamp;
    sscanf(buffer, "%d %d %d %d", &event, &light, &temperature, &timestamp);
    printf("Latest event: %d\n", event);

    switch(event){
        case 0: printf("boot"); break;
        case 1: printf("setup"); break;
        case 2: printf("interval"); break;
        case 3: printf("button"); break;
        case 4: printf("motion"); break;
        default: printf("unknown");
    }

    printf(" (%d)\n", event);
    printf("Temperature is: %.2f\n", temperature / 100.0);
    printf("Light level is: %d\n", light);

    time_t t = timestamp;
    struct tm *lt = localtime(&t);
    printf("Timestamp is :%s", asctime(lt));
}

void handle_exit_request(int sockfd, char *request){ //when given exit this gets executed
    char buffer[BUFFER_SIZE];
    int n;
    
    send (sockfd, request, strlen(request), 0); //send the request to the server
    if(debug){ //if debug is enabled
        buffer[strcspn(buffer, "\n")] = '\0'; //mark end of string
        printf("[DEBUG] sent '%s'\n", request); //print the request
    }
    n = recv(sockfd, buffer, BUFFER_SIZE, 0); //receive the response from the server
    buffer[n] = '\0'; //mark end of string

    if (debug) {
        buffer[strcspn(buffer, "\n")] = '\0'; //mark end of string
        printf("[DEBUG] received '%s'\n", buffer); //print the response
    }

    if (strncmp(buffer, "try again", 9) == 0){ //if the response is try again
        printf("Response: Try again\n"); //print try again
    } else {
        printf("Send verification code: '%s'\n", buffer); //print the verification code
        printf("Enter verification code: "); //print the verification code
        fgets(buffer, BUFFER_SIZE, stdin); //get the verification code
        buffer[strcspn(buffer, "\n")] = 0;

        send(sockfd, buffer, strlen(buffer), 0); //send the verification code to the server
        if (debug) {
            buffer[strcspn(buffer, "\n")] = '\0';
            printf("[DEBUG] sent '%s'\n", buffer); //print the verification code
        }
        n = recv(sockfd, buffer, BUFFER_SIZE, 0); //receive the response from the server
        buffer[n] = '\0';
        if (debug){ //if debug is enabled
            buffer[strcspn(buffer, "\n")] = '\0'; //mark end of string
            printf("[DEBUG] received '%s'\n", buffer); //print the response
        }
        printf("Response: %s\n", buffer);
    }
}

void run_client(int sockfd) { //runs the client
    fd_set readfds; // File descriptor set for select()
    char buffer[BUFFER_SIZE];

    while(1){
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int maxfd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO; //what this does is it sets maxfd to the highest value of the two file descriptors
        
        select(maxfd + 1, &readfds, NULL, NULL, NULL); //selects the file descriptor with the highest value

        if (FD_ISSET(STDIN_FILENO, &readfds)){ //if the file descriptor is set
            fgets(buffer, BUFFER_SIZE, stdin); //get the input from the user
            buffer[strcspn(buffer, "\n")] = 0; //mark end of string
                
            if (strcmp(buffer, "help") == 0){
                print_help();
            } else if (strcmp(buffer, "exit") == 0){
                break;
            } else if (strcmp(buffer, "get") == 0){
                handle_get(sockfd);
            } else if (isdigit(buffer[0])){
                printf("User requested permission\n");
                handle_exit_request(sockfd, buffer);
            } else {
                printf("Unknown command: %s\n", buffer);
            }
            
        }
        if (FD_ISSET(sockfd, &readfds)){ //if the file descriptor is set
            int n = recv(sockfd, buffer, BUFFER_SIZE, 0); //receive the response from the server
            if (n==0){
                printf("server closed connection\n");
            }

            buffer[n] = '\0';

            if (debug) {
                buffer[strcspn(buffer, "\n")] = '\0';
                printf("[DEBUG] received '%s'\n", buffer);
            }

            printf("Received from server: %s\n", buffer);
        }
    }
    close(sockfd);
}

int main(int argc, char *argv[]) {
    char *host;
    int port;
    parse_arguments(argc, argv, &host, &port);
    int sockfd = create_connect_socket(host, port);
    print_help();
    run_client(sockfd);
    return 0;
}