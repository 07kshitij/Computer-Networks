#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m" 

const int N = 100 + 1;

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
  if((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    close(sockFd);
    error("[ERROR] Can't connect to socket");
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portNo);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if(connect(sockFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    close(sockFd);
    error("[ERROR] Unable to connect to server");
  }

  char fileName[] = "word.txt";

  send(sockFd, fileName, strlen(fileName) + 1, 0);

  int fd = open("tcpClient.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(fd < 0){
    error("[ERROR] Can't create file");
  }

  buffer[0] = '\0';
  int total_size;
  int words = 0;
  printf("~~~ Reading from server ~~~\n");
  while((total_size = recv(sockFd, buffer, N, 0))){
    words++;
    if(buffer[0] == '\0'){
      words--; break;
    }
    printf("%d %s\n", words, buffer);
    write(fd, buffer, strlen(buffer));
    for(int i = 0; i < N; i++){
      buffer[i] = '\0';
    }
  }

  if(words == 0){
    printf("FileNotFound\n");
    remove("client.txt");
    close(sockFd);
    close(fd);
    exit(0);
  }

  printf("Total no. of words recvd = %d\n", words);
  close(sockFd);
  close(fd);

  return 0;
}