# UNIX-Socket
Simulating Amazon Web Service using TCP and UDP sockets. 

In this assignment, I construct one client and 4 servers: server A, server B, server C and AWS server.
The client inputs the name of function and value of variable x. 
The function should be LOG or DIV.
LOG = log(1-x)  DIV = 1/(1-x),  0 < x < 1. 
If you input the value of x and the name of function, you will get respective function value.

There are 5 source code files totally.

client.c
The client sets up a TCP connection, and sends the the value of x and the name of function to AWS server.
The client receives the final result which the AWS server sends.

aws.c
The AWS server receives the input which the client sends. 
Then AWS server sets up UDP connections and sends the value of x to server A, B, and C.
The AWS server receives powers of x which server A, B and C sends and uses Taylor Series to calculate the final result.
The AWS server sends the final result to the client.

serverA.c
The server A receives the value of x which the AWS server sends.
The server A sends the square of x to AWS server.

serverB.c
The server B receives the value of x which the AWS server sends.
The server B sends the cube of x to AWS server.

serverC.c
The server C receives the value of x which the AWS server sends.
The server C sends the 5th power of x to AWS server.

I use PHONY in the Makefile.
When you use Ctrl+C to stop the backend servers and the AWS, it will display the following information:
Makefile line: recipe for target 'server' failed
make: *** [server] Interrupt
However, these information will not influence the running of whole program.

References
server.c -- a stream socket server demo   Page 25-27   Beej's Guide to Network Programming
client.c -- a stream socket client demo   Page 27-29   Beej's Guide to Network Programming
listener.c -- a datagram socket server demo   Page 29-31   Beej's Guide to Network Programming
talker.c -- a datagram socket client demo Page 31   Beej's Guide to Network Programming
There are some parts in these demos which I don't need to use and I just comment them.
I modify some parts in these demos.

