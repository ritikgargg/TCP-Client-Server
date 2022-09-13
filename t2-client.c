#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX_LEN 1000	/* Maximum length for the input string */

void log_msg(const char *msg, bool terminate) {
    printf("%s\n", msg);
    if (terminate) exit(-1); /* failure */
}

int main(int argc, char *argv[]){
    if (argc < 3) {
        printf("Usage: %s <server-ip-addr> <server-port>\n", argv[0]);
        printf("where\n");
        printf("\tserver-ip-addr =  The IP address of the server\n");
        printf("\tserver-port = The port number on which the server is listening for connections\n");
        exit(-1);
    }

    /*Create a socket*/
    int sock_fd;
    log_msg("Creating a socket....", false);
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        log_msg("ERROR: Failed to create socket", true);
    }

    struct sockaddr_in server_addr;

    /*Obtain the port number supplied as command line argument*/
    int portno = atoi(argv[2]);
    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);

    int flag = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    if(flag < 0){
        printf("ERROR: Input address %s is not supported\n", argv[1]);
    }
    /*Connect to the server*/
    flag = connect(sock_fd,(struct sockaddr *) &server_addr, sizeof(server_addr));

    log_msg("Connecting to the server....", false);    
    if (flag < 0) 
        log_msg("ERROR: Failed to connect", true); 
    log_msg("SUCCESS: Connected to the server", false);

    char input[MAX_LEN];
    char result[MAX_LEN];

    while (true)
    {   
        /*Read a postfix expression string from the stdin*/
        bzero(input, MAX_LEN);
        printf("Enter a valid postfix expression:\n"); 
        scanf("%[^\n]s", input); 
        getchar();

        /*Supply the input string to the server*/
        log_msg("Sending data to the server....", false); 
        if(send(sock_fd, input, MAX_LEN, 0) < 0){
            log_msg("ERROR: Failed to send data to the server", true);
        }
        log_msg("SUCCESS: Data sent to the server", false);

        /*Exit the client on receiving "exit" in the input*/
        if(strcmp(input, "exit") == 0)
        {
            printf("Closing connection....\n");
            close(sock_fd); /*Close the file descriptor*/
            break;
        }
        /*Obtain the response from the server*/
        bzero(result, MAX_LEN);
        log_msg("Waiting for the response of the server....", false); 
        if(recv(sock_fd, result, MAX_LEN, 0) < 0){
            log_msg("ERROR: Failed to receive the response from the server", true);
        }
        log_msg("SUCCESS: Received response from the server", false);
        printf("Server replied: %s\n", result);       
        fflush(stdin);
    }
    return 0; 
}