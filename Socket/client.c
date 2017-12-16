#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <errno.h>
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>

#define AWSPORT "25360"
#define HOSTNAME "localhost"
#define MAXDATASIZE 100 

// It uses code blocks from Beej's Guide to Network Programming

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr); 
}

int main(int argc, char *argv[]) {

	// set up a TCP connection with aws server and receive the value of function with given input

	if (argc != 3) {
		printf("Please input the function name and the value of input.\n");
		exit(1);
   	}
	char fun[3];
	strcpy(fun,argv[1]);
	double input = atof(argv[2]);

	int sockfd;
	// char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p; 
	int rv;
	double result;
	// char s[INET6_ADDRSTRLEN];
	
    
    memset(&hints, 0, sizeof hints); 
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;
	
	if ((rv = getaddrinfo(HOSTNAME, AWSPORT, &hints, &servinfo)) != 0) { 
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) { 
			close(sockfd);
			perror("client: connect");
			continue; 
		}
		break; 
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
        return 2;
	}

	// inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

	printf("The client is up and running.\n"); 

	freeaddrinfo(servinfo);
	
	send(sockfd, fun, sizeof fun, 0);
	send(sockfd, (char *)&input, sizeof input, 0);

	printf("The client sent <%g> and %s to AWS.\n", input, fun);


	if (recv(sockfd, (char *)&result, sizeof result, 0) == -1) { 
		perror("recv");
		exit(1); 
	}

	// buf[numbytes] = '\0';

	printf("According to AWS, %s on <%g>: <%g>.\n",fun, input, result);

	close(sockfd);
	
	return 0; 
}