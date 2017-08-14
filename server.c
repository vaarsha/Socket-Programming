#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res;
	int sockfd, newfd, status;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; //change to ipv4
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	if (status = get(addrinfo(NULL, "3490", &hints, &res)) != 0) {
			fprintf(stderr, "getaddrinfo error: %s\n",
				gai_strerror(status));
			exit(1);
	}

	freeaddrinfo(res);
	return 0;
}

