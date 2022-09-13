# TCP Client Server


## Task 1

### Description
 Build a connection-oriented (TCP) client server model. Client sends the string to the server and server reverses the string sent by the client and sends it back to the client.

### Implementation

##### To set up the server
&nbsp;&nbsp;&nbsp;&nbsp;- To compile the server:
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`gcc t1-server.c -o server`

&nbsp;&nbsp;&nbsp;&nbsp;- To execute the server:
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./server <port-number>`
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;where,
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`port-number` = The port number on which the server will accept connections

##### To set up the client
&nbsp;&nbsp;&nbsp;&nbsp;- To compile the client:
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`gcc t1-client.c -o client`

&nbsp;&nbsp;&nbsp;&nbsp;- To execute the client:
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./client <server-ip-addr> <server-port>`
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;where,
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`server-ip-addr` =  The IP address of the server
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`server-port` = The port number on which the server is listening for connections

##### Commands for a sample run
```
    gcc t1-server.c -o server
    ./server 8000

    gcc t1-client.c -o client
    ./client "127.0.0.1" 8000
```


## Task 2

### Description
 Build a connection-oriented (TCP) client server model. Client sends the string to the server and server reverses the string sent by the client and sends it back to the client.

### Implementation
The client connects to the server, and then asks the user for input. The user enters a simple arithmetic expression string in postfix form (e.g., `1 2 +`, `5 6 22.3 * +`). The user's input is sent to the server via the connected socket.

The server reads the user's input from the client socket, evaluates the postfix expression, and sends the result back to the client as well as writes the following in a file named `server_records.txt` in the following format:

`<client_id> <query> <answer> <time_elapsed>`

##### To set up the server
&nbsp;&nbsp;&nbsp;&nbsp;- To compile the server:
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`gcc t2-server.c -o server -lpthread`

&nbsp;&nbsp;&nbsp;&nbsp;- To execute the server:
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./server <port-number>`
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;where,
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`port-number` = The port number on which the server will accept connections

##### To set up the client
&nbsp;&nbsp;&nbsp;&nbsp;- To compile the client:
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`gcc t2-client.c -o client`

&nbsp;&nbsp;&nbsp;&nbsp;- To execute the client:
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./client <server-ip-addr> <server-port>`
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;where,
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`server-ip-addr` =  The IP address of the server
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`server-port` = The port number on which the server is listening for connections

**To set up multiple clients**, create executables with different names. For example:
```
    gcc t2-client.c -o client1
    gcc t2-client.c -o client2
etc.
```

##### Commands for a sample run
```
    gcc t2-server.c -o server -lpthread
    ./server 8001

    gcc t2-client.c -o client
    ./client "127.0.0.1" 8001
```

**Note:**
(i) To exit the client program, give "exit" in the input.
```
        Enter a valid postfix expression:
        exit
```
    
(ii) Make sure that the input postfix expression is valid, following the format of the question and without any undefined behaviour.

(iii) Make sure that the required command line arguments are provided while running the executables.

(iv) The race condition while writing to the `server_records.txt` file at the server, is handled by using mutex lock.

(v) The values of the macros(`MAX_LEN`, `BACKLOG`, etc) in the codes can be changed as per the requirement.

(vi) Make sure that `server_records.txt` file exists in the directory, before running task-2 server.