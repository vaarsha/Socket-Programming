#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#define PORTNO "3490"
#define BACKLOG 5

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res;
	struct sockaddr_in client;
	int sockfd, newsockfd, status, yes =1;
	socklen_t sin_size;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;    //AF_UNSPEC
	hints.ai_flags = AI_PASSIVE; //address of local host
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
			fprintf(stderr, "getaddrinfo error: %s\n",
				gai_strerror(status));
			exit(1);
	}

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1)
		perror("socket error\n");
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
		       sizeof(int)) == -1)
		perror("setsockopt");

	if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		close(sockfd);
		perror("bind failed");
	}

	//connect(sockfd, res->ai_addr, res->ai_addrlen);
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	printf("Waiting for connections\n");
	while(1) {
		sin_size = sizeof(client);
		newsockfd = accept(sockfd, (struct sockaddr *) &client,
				   &sin_size);
		if (newsockfd == -1) {
			perror("accept");
			continue;
		}

		printf("Connection accepted\n");
		if (!fork()) {
			close(sockfd);
			if (send(newsockfd, "Hello, world!", 13, 0) == -1)
				perror("send");
			close(newsockfd);
			exit(0);
		}
		close(newsockfd);
	}

	freeaddrinfo(res);
	return 0;
}
