#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <time.h> 

#define MAX_LEN 2000
#define BACKLOG 5	
#define ID_LEN 100

int sock_fd;
pthread_mutex_t mutex;  /* Mutex lock */
clock_t start_time; /*Variable to store the start time of the server*/

/*Struct to pass an argument to the client handler function*/
typedef struct clientParam{
    int sock_fd;
    int client_id;
} clientParam;

typedef struct operand{
    double value;
    bool isDoubleType;   /* type == false indicates integer type, type == true indicates double type */
} operand;

void log_msg(const char *msg, bool terminate) {
    printf("%s\n", msg);
    if (terminate) exit(-1); /* failure */
}

/*Function to handle the signal SIGINT(Ctrl+C).*/
void sig_handler(int signum){
    pthread_mutex_destroy(&mutex);  /* Destroying the mutex */
    log_msg("Closing connection...", false);
    close(sock_fd); /*Close the connection*/
    exit(0);
}

/*Function to evaluate the postix expression present in the buffer*/
operand evaluate_postfix_expression(char *buffer){
    /*Stack to store operands to assist in the postfix expression evaluation*/
    operand stack[MAX_LEN];
    int top = -1, i, j;
    char tmpStr[MAX_LEN];
    bool isDoubleType;

    int len = strlen(buffer);
    for(i = 0; i < len;){
        if(buffer[i] == ' ')    /*Skip whitespaces*/
            i++;
        else if(buffer[i] >= '0' && buffer[i] <= '9'){
            /*Handling operands*/
            j = 0;
            isDoubleType = false;
            while(i < len && buffer[i] != ' '){
                 /*Check for the presence of '.' in the operand*/
                if(buffer[i] == '.'){
                    isDoubleType = true; /*In case a '.' is present in an operand, it is considered of double type*/
                }
                tmpStr[j++] = buffer[i++];
            }
            tmpStr[j] = '\0';
            operand tmp;
            tmp.value = atof(tmpStr);
            tmp.isDoubleType = isDoubleType;
            stack[++top] = tmp; /*Push the operand to the stack*/
        }else{
            /*Handling operators*/

            /*For every operator, pop two operands from the stack*/
            operand num2 = stack[top--];
            operand num1 = stack[top--];
            operand tmpResult;

            if(!num1.isDoubleType && !num2.isDoubleType){   /*If both the operands are integers, then the result is also an integer*/
                tmpResult.isDoubleType = false;
                if(buffer[i] == '+')
                    tmpResult.value = (int)(num1.value) + (int)(num2.value);
                else if(buffer[i] == '-') 
                    tmpResult.value = (int)(num1.value) - (int)(num2.value);
                else if(buffer[i] == '*') 
                    tmpResult.value = (int)(num1.value) * (int)(num2.value);
                else if(buffer[i] == '/') 
                    tmpResult.value = (int)(num1.value) / (int)(num2.value);
            }else{
                /*If any of the operands is double(or float) type, then the result of the operation will also be double(or float)*/
                tmpResult.isDoubleType = true;
                if(buffer[i] == '+')
                    tmpResult.value = num1.value + num2.value;
                else if(buffer[i] == '-') 
                    tmpResult.value = num1.value - num2.value;
                else if(buffer[i] == '*') 
                    tmpResult.value = num1.value * num2.value;
                else if(buffer[i] == '/') 
                    tmpResult.value = num1.value / num2.value;
            }
            stack[++top] = tmpResult;   /*Push the result to the stack*/
            i++;
        }
    }
    return stack[top];
}

/*Function to handle the worker thread created for each client connection*/
void* client_handler(void *arg){
    clientParam *para = (clientParam*)(arg);
    char input[MAX_LEN];
    char output[MAX_LEN];
    operand result;
    clock_t end_time;
    double time_taken;
    while(true){ 
        /*Read the data(postfix expression) supplied by the client*/
        bzero(input, MAX_LEN);
        if(recv(para->sock_fd, input, MAX_LEN, 0) < 0){
            printf("ERROR: Failed to receive data or connection terminated by the client %d\n", para->client_id);
            break;
        }

        /*Break the while loop, if the client wishes to close the connection*/
        if(strcmp(input,"exit") == 0){
            printf("Terminating the worker thread for the client %d\n", para->client_id);
            break;
        }
        
        printf("Received a request from the client %d: %s\n", para->client_id, input);
        /*Determine the result of the postfix expression*/
        result = evaluate_postfix_expression(input);
        
        /*Send the result of the postfix expression back to the client*/
        bzero(output, MAX_LEN);
        if(!result.isDoubleType)
            sprintf(output, "%d", (int)(result.value));
        else
            sprintf(output, "%f",result.value);
        
        if(send(para->sock_fd, output, strlen(output), 0) < 0){
            log_msg("ERROR: Failed to send data to the client", true);
        }
        printf("Sent a response to the client %d: %s\n", para->client_id, output);
        end_time = clock() - start_time;
        time_taken = ((double)end_time)/(CLOCKS_PER_SEC/1000);
        
        /*Handling race condition*/
        pthread_mutex_lock(&mutex); /* Acquiring the mutex lock */

        /*Logging the record into the server_records.txt file*/
        FILE *fp = fopen("server_records.txt", "a");
		fprintf(fp, "%d  %s  %s  %fms\n", para->client_id, input, output, time_taken);                
    	fclose(fp);
        pthread_mutex_unlock(&mutex);   /* Releasing the mutex lock */
    }
    free(arg);
    pthread_exit(NULL); /*Exiting the thread*/
    return NULL;
} 
int main(int argc, char *argv[]){

    /*Checking if the required parameters are supplied in command line arguments*/
    if (argc < 2) {
        printf("Usage: %s <port-number>\n", argv[0]);
        printf("where <port-number> = The port number on which the server will accept connections\n");
        exit(-1);
    }

    signal(SIGINT, sig_handler); /* Register signal handler for SIGINT*/

    int newsock_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char client_ip_port[ID_LEN];
    int client_cnt = 1; /*Counter to serially assign client IDs*/
    
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

    FILE *fp;
    fp = fopen("server_records.txt", "a");
    fprintf(fp, "\n============================ New Session ============================\n\n");
    fclose(fp);

    pthread_mutex_init(&mutex, NULL); /*Initializing the mutex lock to address the race condition between client threads*/

    start_time = clock();
    printf("Waiting for incoming connections...\n");
    while (true)
    {
        /*Accept client connections*/
        newsock_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &client_len);
        if(newsock_fd < 0){
            log_msg("ERROR: Failed to accept connection from client", true);
        }else{
            sprintf(client_ip_port, "%s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            printf("Client %s connected\n", client_ip_port);
            
            /*Create a worker thread for every client*/
            pthread_t thr_id;
            clientParam *para = (clientParam*) malloc(sizeof(clientParam));
            para->sock_fd = newsock_fd;
            para->client_id = client_cnt;
            printf("Creating a worker thread for client %s, assigning an ID %d\n", client_ip_port, client_cnt);
            int rc = pthread_create(&thr_id, NULL, client_handler, (void *)(para));
            if (rc) {
                printf("Failed to create thread for client %s. Continuing to next....\n", client_ip_port);
            }
            client_cnt += 1;
        }
    }    
    return 0;
}