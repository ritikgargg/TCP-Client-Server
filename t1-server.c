#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h> 
#include <sys/socket.h>

#define MAX_LEN 1000    /* Maximum length for the input/output */
#define BACKLOG 5	/* Maximum number of pending connections */


void log_msg(const char *msg, bool terminate) {
    printf("%s\n", msg);
    if (terminate) exit(-1); /* failure */
}

int main(int argc, char *argv[]){
    /*Checking if the required parameters are supplied in command line arguments*/
    if (argc < 2) {
        printf("Usage: %s <port-number>\n", argv[0]);
        printf("where <port-number> = The port number on which the server will accept connections\n");
        exit(-1);
    }

    int sock_fd, newsock_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[MAX_LEN] = {0};

    /*Create a socket*/    
    log_msg("Creating a socket....", false);
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        log_msg("Failed to create socket", true);
    }

    /*Obtain the port number supplied as command line argument*/
    int portno = atoi(argv[1]);
    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portno);

    /*Bind the socket to the address*/
    log_msg("Binding socket to the address....", false);
    if(bind(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        log_msg("ERROR: Failed to bind the socket", true);
    }

    /*Listen for incoming connections*/
    log_msg("Listening for connections....", false);
    if(listen(sock_fd, BACKLOG) < 0){
        log_msg("Failed to listen on the socket for connections", true);
    }

    log_msg("Waiting to accept a client connection....", false);
    /*Accept a client connection*/
    newsock_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &client_len);
    if(newsock_fd < 0){
        log_msg("ERROR: Failed to accept connection from client", true);
    }
    log_msg("SUCCESS: Client Connected", false);
    
    /*Read the data(string) supplied by the client*/
    log_msg("Receiving data from the client...", false);
    if(recv(newsock_fd, buffer, sizeof(buffer), 0) < 0){
        log_msg("ERROR: Failed to receive data from the client", true);
    }
    log_msg("SUCCESS: Data received from the client", false);

    /*Reverse the string supplied by the client*/
    char tmp;
    int len = strlen(buffer);
    for(int i = 0; i < len/2; i++){
        tmp = buffer[i];
        buffer[i] = buffer[len - i - 1];
        buffer[len - i - 1] = tmp;
    }

    /*Send the reversed string back to the client*/
    log_msg("Sending data to the client...", false);
    if(send(newsock_fd, buffer, sizeof(buffer), 0) < 0){
        log_msg("ERROR: Failed to send data to the client", true);
    }
    log_msg("SUCCESS: Data sent to the client", false);

    /*Close the connections*/
    log_msg("Closing connection...", false);
    close(newsock_fd);
    close(sock_fd);
    return 0;
}