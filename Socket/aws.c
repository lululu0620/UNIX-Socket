#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define TCPPORT "25360" // TCP port number of aws server
#define UDPPORT "24360" // UDP port number of aws server
#define APORT "21360" // UDP port number of server A
#define BPORT "22360" // UDP port number of server B
#define CPORT "23360" // UDP port number of server C
#define HOSTNAME "localhost"
#define BACKLOG 10 

// It uses code blocks from Beej's Guide to Network Programming

void sigchld_handler(int s) {
	while(waitpid(-1, NULL, WNOHANG) > 0); 
}

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
   	return &(((struct sockaddr_in6*)sa)->sin6_addr); 
}

double sum(char fun[], double input, double value1, double value2, double value3, double value4, double value5) {
	// calculate the value of the given function with powers received from other servers.
	double sum;
	if (strcmp(fun,"LOG") == 0) {
		sum = -input-value1/2.0-value2/3.0-value3/4.0-value4/5.0-value5/6.0;
	}
	else {
		sum = 1+input+value1+value2+value3+value4+value5;
	}
	return sum;
}

double calculate(double input, char server) {

	// set up UDP connections with backend-servers A, B and C and receive the powers
	// It uses code blocks from Beej's Guide to Network Programming

	int sockfd;
	struct addrinfo hints, *servinfo, *p;
   	int rv;
	char* serverPort;
	double result;
	memset(&hints, 0, sizeof hints); 
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_DGRAM;

	if (server == 'A') serverPort = APORT;
	if (server == 'B') serverPort = BPORT;
	if (server == 'C') serverPort = CPORT;

	if ((rv = getaddrinfo(HOSTNAME, serverPort, &hints, &servinfo)) != 0) { 
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue; 
		}
		break; 
	}
	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);

	if (sendto(sockfd, (char *)&input, sizeof input, 0, p->ai_addr, p->ai_addrlen) == -1) {
		perror("talker: sendto");
		exit(1); 
	}
	
	printf("The AWS sent <%g> to Backend-Server %c.\n", input, server); 
	recvfrom(sockfd, (char *)&result, sizeof result, 0, NULL, NULL);
	printf("The AWS received <%g> Backend-Server %c using UDP over port <%s>.\n", result, server, UDPPORT); 
	// close(sockfd);
	
	return result;
}

int main(void) {

	// set up TCP connections with the client and receive the input and function name
	// It uses code blocks from Beej's Guide to Network Programming
	int sockfd, new_fd; 
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr;  
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	//char s[INET6_ADDRSTRLEN]; 
	int rv;

	memset(&hints, 0, sizeof hints); 
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE; 

	if ((rv = getaddrinfo(NULL, TCPPORT, &hints, &servinfo)) != 0) { 
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
        }
     	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1); 
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) { 
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n"); 
		return 2;
	}

	freeaddrinfo(servinfo);

	if (listen(sockfd, BACKLOG) == -1) { 
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) { 
    	perror("sigaction");
		exit(1);
	}
	printf("The AWS is up and running.\n");

	while(1) {
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) { 
			perror("accept"); 
			continue;
		}
		
		// inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
		// printf("server: got connection from %s\n", s);

		char fun[3];
		double input;
		recv(new_fd, fun, sizeof fun, 0);
		recv(new_fd, (char *)&input, sizeof input, 0);

		printf("The AWS received input <%g> and function=%s from the client using TCP over port <%s>.\n", input, fun, TCPPORT);
		double value1 = calculate(input, 'A');
		double value2 = calculate(input, 'B');
		double value4 = calculate(input, 'C');
		double value3 = calculate(value1, 'A');
		double value5 = calculate(value1, 'B');
		printf("Values of powers received by AWS: <%g, %g, %g, %g, %g, %g>.\n", input, value1, value2, value3, value4, value5); 
		double result = sum(fun, input, value1, value2, value3, value4, value5);
		printf("AWS calculated %s on <%g>: <%g>.\n", fun, input, result);
		send(new_fd, (char *)&result, sizeof result, 0);
		printf("The AWS sent <%g> to client.\n", result);
		close(new_fd);

	}
	return 0;
}