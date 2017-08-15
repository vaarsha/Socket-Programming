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
//	char *msg = "Hey server whatsup!";

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

/*	len = strlen(msg);
	numbytes = send(sockfd, msg, len, 0);
	if (numbytes == -1)
		perror("send error");
*/
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

	freeaddrinfo(res);
	close(sockfd);
	return 0;
}
