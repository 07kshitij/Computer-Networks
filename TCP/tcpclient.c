#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[]){
	
	struct sockaddr_in server_addr; // Same as server
	struct hostent *server; // Defines a host on the computer, included in netdb.h
	int sockFd, portNo, n;
	char buffer[2048];

	if(argc < 3){ // argv[1] has the hostname, argv[2] the portNo
		fprintf(stderr, "Use %s <hostname> <port>\n", argv[0]);
		exit(1);
	}
	portNo = atoi(argv[2]);
	sockFd = socket(AF_INET, SOCK_STREAM, 0); // Create socket at client
	if(sockFd < 0){
		perror("ERROR Opening socket\n");
		exit(1);
	}
	server = gethostbyname(argv[1]); // Returns pointer to a hostent with info about server
	if(server == NULL){
		perror("ERROR, no such host\n");
		exit(1);
	}
	bzero((char *) &server_addr, sizeof(server_addr)); // same as server

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portNo);
	bcopy((char *)server -> h_addr, (char *)&server_addr.sin_addr.s_addr, server -> h_length);
	// similar to strcpy(char *s, char *t, strlen(s)), copies s to t\
		here copies the address of server in server -> h_addr to server_addr field

	if(connect(sockFd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		perror("ERROR connecting");
		exit(1);
	}
	// connect(fileDescriptor of socket (here client), address of host to connect , sizeof that address)
	// client needs to know portno of server, but its own port no is assigned as per availability
	char message[2408];
	while(1){
		printf("Enter Message >> ");
		bzero(message, sizeof(message));
		fgets(message, sizeof(message), stdin);
		n = write(sockFd, message, strlen(message) + 1);
		// write to server
		if(n < 0){
			perror("ERROR writing to socket\n");
			exit(1);
		}
		bzero(buffer, sizeof(buffer));
		n = read(sockFd, buffer, sizeof(buffer));
		// read from server
		if(n < 0){
			perror("ERROR reading from socket\n");
			exit(1);
		}
		printf("Reading from %s:%d >> ", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
		printf("%s\n", buffer);
	}
	close(sockFd);

	return 0;
}