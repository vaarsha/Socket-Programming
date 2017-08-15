#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#define PORTNO "3490"
#define MAXBUFLEN 100

int main()
{
	struct addrinfo hints, *res;
	int sockfd,len, numbytes, status;
	char buf[MAXBUFLEN];
	char msg[MAXBUFLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(NULL, PORTNO, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n",
			gai_strerror(status));
		exit(1);
	}

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1)
		perror("socket client");

	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		close(sockfd);
		perror("Connect error");
	}

	numbytes = recv(sockfd, buf, MAXBUFLEN-1, 0);
		
	if (numbytes == -1) {
		perror("receive error");
		exit(1);
	}
		
	if (numbytes == 0) {
		printf("connection closed");
		exit(1);
	}
		
	buf[numbytes] = '\0';
	printf("Message from server '%s' \n", buf);

	while(1) {
		
		numbytes = recv(sockfd, buf, MAXBUFLEN-1, 0);
			
		if (numbytes == -1) {
			perror("receive error");
			exit(1);
		}
			
		if (numbytes == 0) {
			printf("connection closed");
			exit(1);
		}
			
		buf[numbytes] = '\0';

		printf("message received is '%s' \n", buf);

		printf("Enter the result: ");
		fgets(msg, MAXBUFLEN, stdin);
			
		if(strncmp(msg, "stop", 4) == 0) {
			if (send(sockfd, msg, strlen(msg), 0) == -1)
				perror("send");
			break;
		}
                        
		if (send(sockfd, msg, strlen(msg), 0) == -1)
			perror("send");
	}
		
	numbytes = recv(sockfd, buf, MAXBUFLEN-1, 0);
	
	if (numbytes == -1) {
		perror("receive error");
		exit(1);
	}
	
	if (numbytes == 0) {
		printf("connection closed");
		exit(1);
	}
		
	buf[numbytes] = '\0';
	printf("message received is '%s' \n", buf);

	close(sockfd);
	freeaddrinfo(res);
	return 0;
}
