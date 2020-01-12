#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>  

const int N = 10 + 1;

void error(char *message){
	perror(message);
	exit(0);
}

int main(int argc, char const *argv[]){
	
	int sockFd, portNo, newsockFd;
	char buffer[N];
	socklen_t clilen;

	struct sockaddr_in server_addr, client_addr;

	if(argc < 2){
		error("[ERROR] Pls specify portNo");
	}
	portNo = atoi(argv[1]);
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockFd < 0){
		close(sockFd);
		error("[ERROR] Can't create socket");
	}

	bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portNo);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sockFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		close(sockFd);
		error("[ERROR] Can't bind the socket");
	}

	listen(sockFd, 5);

	clilen = sizeof(client_addr);
	newsockFd = accept(sockFd, (struct sockaddr*) &client_addr, &clilen);
	if(newsockFd < 0){
		close(sockFd);
		error("[ERROR] Connection error");
	}

	printf("~~~ Connected to client ~~~\n");
	printf("~~~ Reading from %s : %d ~~~\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	char fileName[N];
	bzero(fileName, sizeof(fileName));
	int n = recv(newsockFd, fileName, N, 0);
	if(n < 0){
		close(sockFd);
		close(newsockFd);
		error("[ERROR] Can't read from socket");
	}

	int fd = open(fileName, O_RDONLY);
	if(fd < 0){
		close(sockFd);
		close(newsockFd);
		error("[ERROR] FileNotFound");
	}

	printf("~~~ Reading from file : %s ~~~\n", fileName);

	int r;
	do{
		char temp[N];
		r = read(fd, temp, N - 1);
		temp[r] = '\0';
		send(newsockFd, temp, r, 0);
	}while(r == N - 1);

	close(sockFd);
	close(newsockFd);
	close(fd);

	return 0;
}