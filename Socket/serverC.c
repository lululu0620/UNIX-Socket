#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#define MYPORT "23360" 

// It uses code blocks from Beej's Guide to Network Programming

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr); 
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr); 
}

int main(void) {

	// set up a UDP connection with aws server and send the 5th power of input
	
	int sockfd;
    struct addrinfo hints, *servinfo, *p;
   	int rv;
	//int numbytes;
	struct sockaddr_storage their_addr; 
	double input; 
	double power;
	socklen_t addr_len;
	// char s[INET6_ADDRSTRLEN];
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM; 
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) { 
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("ServerC: socket");
			continue; 
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) { 
			close(sockfd);
			perror("ServerC: bind");
            continue;
   		}
   		break; 
   	}

	if (p == NULL) {
		fprintf(stderr, "ServerC: failed to bind socket\n"); 
		return 2;
	}

	freeaddrinfo(servinfo);

	printf("The Server C is up and running using UDP on port <%s>.\n", MYPORT); 

	while(1) {
		addr_len = sizeof their_addr;
		if (recvfrom(sockfd, (char *)&input, sizeof input , 0, (struct sockaddr *)&their_addr, &addr_len) == -1) {
			perror("recvfrom");
			exit(1); 
		}
		// printf("listener: got packet from %s\n", inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr), s, sizeof s)); 
		// printf("listener: packet is %d bytes long\n", numbytes); 
		// buf[numbytes] = '\0';
		printf("The Server C received input <%g>.\n", input);
		power = input*input*input*input*input;
	    printf("The Server C calculated 5th power: <%g>\n", power);
	    sendto(sockfd, (char *)&power, sizeof power, 0, (struct sockaddr *)&their_addr, addr_len);
	    printf("The Server C finished sending the output to AWS.\n");
		// close(sockfd); 
	}
	return 0;
}
