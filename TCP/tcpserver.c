#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[]){
	
	int sockFd, newsockFd, portNo, numChar, n;
	char buffer[2048];
	socklen_t clilen;

	struct sockaddr_in server_addr, client_addr;

	if(argc < 2){ 
		perror("ERROR, no port provided\n");
		exit(1);
	}

	sockFd = socket(AF_INET, SOCK_STREAM, 0); // gets the socket file descriptor
	// socket(domain, type, protocol)
	// domain -> Protocol family to be used, IPv4, IPv6 etc ~ AF_INET
	// type -> Communication semantics (protocols) ~ SOCK_STREAM (Read data as stream of chars) or SOCK_DGRAM (Read data as chunks)
	// protocol -> particular protocol to be used with the given domain and type ~ 0
	if(sockFd < 0){
		perror("ERROR opening socket\n");
		exit(1);
	}
	bzero((char *) &server_addr, sizeof(server_addr)); // sets the buffer to all 0's

	portNo = atoi(argv[1]); // Port no. at which server is listening

	server_addr.sin_family = AF_INET; // Same as socket() domain
	server_addr.sin_addr.s_addr = INADDR_ANY; // IP address of host (same as local machine IP address for server)
	server_addr.sin_port = htons(portNo); // convert host byte order to network byte order

	if(bind(sockFd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		perror("ERROR on Binding\n");
		exit(1);
	}
	// binds the server address to the socket

	listen(sockFd, 5); // 5 is the max no. of blocklisted (waiting) connections


	while(1){
		clilen = sizeof(client_addr);
		newsockFd = accept(sockFd, (struct sockaddr *) &client_addr, &clilen);
		if(newsockFd < 0){
			perror("ERROR on Accept\n");
			continue;
		}
		printf("New connection from %s:%d \n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		while(1){
			bzero(buffer, sizeof(buffer));
			if((n = read(newsockFd, buffer, sizeof(buffer))) < 0){
				perror("ERROR reading from socket\n");
				break;				
			}
			if(n == 0){
				close(newsockFd); break;
			}
			printf("\t>> ");
			printf("Server recv : %s\n", buffer);
			n = write(newsockFd, buffer, sizeof(buffer));
			if(n < 0){
				perror("ERROR writing to socket\n");
				continue;
			}		
		}
	}
	close(sockFd);

	return 0;
}