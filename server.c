#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include<time.h>

#define PORTNO "3490"
#define BACKLOG 5
#define MAXDATASIZE 100

int calculate(int number1, char operator, int number2)
{
	int result = 0;

	switch(operator) {
		case '+': result = number1 + number2;
			  break;
		case '-': result = number1 - number2;
			  break;
		case '*': result = number1 * number2;
			  break;
		case '/': result = number1 / number2;
			  break;
		default: printf("Invalid operator");
	}
	return result;
}

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res;
	struct sockaddr_in client;
	int sockfd, newsockfd, status, numbytes, yes =1;
	socklen_t sin_size;
	char buf[MAXDATASIZE], opt, strtoint[MAXDATASIZE];
	char op[] = { '+', '-', '*', '/'};
	int num1, num2, output, retval, score = 0;
	time_t t;
	char opbuf[MAXDATASIZE];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;    //AF_UNSPEC
	hints.ai_flags = AI_PASSIVE; //address of local host
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(NULL, PORTNO, &hints, &res)) != 0) {
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

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen failed");
		exit(1);
	}
	printf("Waiting for connections\n");

	sin_size = sizeof(client);
	newsockfd = accept(sockfd, (struct sockaddr *) &client, &sin_size);
	
	if (newsockfd == -1) {
		perror("accept");
		exit(1);
	}

	printf("Connection accepted\n");
	
	close(sockfd);
	
	if (send(newsockfd, "Solve the following problems", 31, 0) == -1)
		perror("send");
	
	srand((unsigned) time(&t));
	
	while(1) {	
		num1 = rand()%20;
		num2 = rand()%20;
		opt = op[rand()%4];
		output = calculate(num1, opt, num2);

		sprintf(strtoint," %d %c %d",num1, opt, num2);
		printf("the sent eqution is %s\n",strtoint);

		if(send(newsockfd, strtoint, strlen(strtoint), 0) == -1) {
				close(newsockfd);
				perror("send");
				exit(1);
		}
		
		if ((numbytes = recv(newsockfd, buf, MAXDATASIZE-1, 0)) == -1) {
				close(newsockfd);
				perror("receiver");
				exit(1);
		}
		
		if (numbytes == 0) {
			close(newsockfd);
			perror("connection closed");
			exit(1);
		}

		buf[numbytes] = '\0';
		if (strncmp(buf,"stop", 4) == 0)
			break;

		retval = atoi(buf);

		if (retval == output) {
			printf("Correct answer\n");
			score++;
		} else {
			printf("Incorrect answer\n");
			score--;
		}
	}
	
out:
	sprintf(strtoint,"%d",score);	
	if (send(newsockfd, "Bye and your final score is ", 25, 0) == -1)
		perror("send");
	
	if (send(newsockfd, strtoint, strlen(strtoint), 0) == -1)
		perror("send");
		
	close(newsockfd);
	freeaddrinfo(res);

	return 0;
}
