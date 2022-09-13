#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_LEN 1000	/* Maximum length for the input array */

void log_msg(const char *msg, bool terminate) {
    printf("%s\n", msg);
    if (terminate) exit(-1); /* failure */
}

int main(int argc, char *argv[]){
    /*Checking if the required parameters are supplied in command line arguments*/
    if (argc < 3) {
        printf("Usage: %s <server-ip-addr> <server-port>\n", argv[0]);
        printf("where\n");
        printf("\tserver-ip-addr =  The IP address of the server");
        printf("\tserver-port = The port number on which the server is listening for connections");
        exit(-1);
    }

    /*Create a socket*/
    int sock_fd;
    log_msg("Creating a socket....", false);
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        log_msg("ERROR: Failed to create socket", true);
    }


    struct sockaddr_in server_addr;

    int portno = atoi(argv[2]); /*Obtain the port number supplied as command line argument*/
    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);

    int flag = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    if(flag < 0){
        log_msg("ERROR: Input address is not supported", true);
    }
    /*Connect to the server*/
    flag = connect(sock_fd,(struct sockaddr *) &server_addr, sizeof(server_addr));

    log_msg("Connecting to the server....", false);    
    if (flag < 0) 
        log_msg("ERROR: Failed to connect", true); 
    log_msg("SUCCESS: Connected to the server", false);

    /*Read a string from the stdin*/
    char buffer[MAX_LEN];
    printf("Enter a string to be sent to the server:\n");
    scanf("%[^\n]s", buffer);

    /*Supply the input string to the server*/
    log_msg("Sending data to the server....", false); 
    if(send(sock_fd, buffer, sizeof(buffer), 0) < 0){
        log_msg("ERROR: Failed to send data to the server", true);
    }
    log_msg("SUCCESS: Data sent to the server", false);

    /*Obtain the response from the server*/
    log_msg("Waiting for the response of the server....", false); 
    if(recv(sock_fd, buffer, sizeof(buffer), 0) < 0){
        log_msg("ERROR: Failed to receive the response from the server", true);
    }
    log_msg("SUCCESS: Received response from the server", false);
    printf("%s\n", buffer);
    log_msg("Closing connection...", false);
    close(sock_fd); /*Close the file descriptor*/
    return 0; 
}