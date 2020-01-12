#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>  

const int N = 10 + 1;

void error(char *message){
	perror(message);
	exit(0);
}

int main(int argc, char const *argv[]){

	int sockFd, portNo;
	char buffer[N];
	struct sockaddr_in server_addr, client_addr;

	if(argc < 3){
		error("[ERROR] Pls specify hostname and portNo");
	}

	portNo = atoi(argv[2]);
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockFd < 0){
		error("[ERROR] Can't open socket");
	}

	bzero((char*)&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portNo);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(connect(sockFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		error("[ERROR] Unable to connect to server");
	}

	char fileName[N];
	printf("Enter fileName : ");
	scanf("%s", fileName);

	send(sockFd, fileName, strlen(fileName) + 1, 0);
	int fd = open("client.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(fd < 0){
		error("[ERROR] Can't create file");
	}

	bzero(buffer, sizeof(buffer));
	int words = 0, bytes = 0, m, cnt = 0;
	int i, j;
	bool delimiter = 0;

	while((m = recv(sockFd, buffer, N, 0)) > 0){
		cnt++;
		printf("%s\n", buffer);
		write(fd, buffer, strlen(buffer));
		for(j = 0; j < m; j++){
			bytes++;
			if(buffer[j] == ','){
				delimiter = 1;
			}
			else if(buffer[j] == ';'){
				delimiter = 1;
			}
			else if(buffer[j] == ':'){
				delimiter = 1;
			}
			else if(buffer[j] == '.'){
				delimiter = 1;
			}
			else if(buffer[j] == ' '){
				delimiter = 1;
			}
			else if(buffer[j] == '\t'){
				delimiter = 1;
			}
			else{
				if(delimiter){
					words++; delimiter = 0;
				}
			}
		}
		if(delimiter)
			words++;
		bzero(buffer, sizeof(buffer));
	}

	if(cnt == 0 && m == 0){
		remove("client.txt");
		printf("FileNotFound\n");
		close(sockFd);
		close(fd);
		exit(0);
	}

	printf("[SUCCESS] Received %d number of bytes and %d number of words\n",bytes, words);
	close(sockFd);
	close(fd);
	return 0;
}