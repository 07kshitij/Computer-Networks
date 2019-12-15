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

const int N = 2048;
const int MAX_WORDS = 10;
const int MAX_LEN = 50;

void error(char *message){
  fprintf(stderr, "%s >> ", "ERROR");
  fprintf(stderr, "%s\n", message);
}

void do_read(int sockFd, char *buffer, struct sockaddr_in *client_addr){
  socklen_t len;
  len = sizeof(*client_addr);
  int n = recvfrom(sockFd, (char *)buffer, N, 0, (struct sockaddr*)client_addr, &len);
  if(n < 0){
    error("READ Error");
    exit(1);
  }
  buffer[n] = '\0';
}

void do_write(int sockFd, char message[], struct sockaddr_in *client_addr){
  int n = sendto(sockFd, (const char *)message, strlen(message), 0, (const struct sockaddr*)client_addr, sizeof(*client_addr));
  if(n < 0){
    error("WRITE Error");
    exit(1);
  }
}

int main(int argc, char const *argv[]){

  int sockFd, portNo;
  char buffer[N];

  struct sockaddr_in server_addr, client_addr;

  if(argc < 3){
    fprintf(stderr, "Use %s <hostname><port>\n", argv[0]);
    exit(1);
  }

  portNo = atoi(argv[2]);
  sockFd = socket(AF_INET, SOCK_DGRAM, 0);

  if(sockFd < 0){
    error("Socket can't be opened");
    exit(1);
  }

  bzero((char*) &server_addr, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portNo);
  server_addr.sin_addr.s_addr = INADDR_ANY; 

  char fileName[MAX_LEN];
  printf("Enter File name :: "); 
  scanf("%s", fileName);
  do_write(sockFd, fileName, &server_addr);
  do_read(sockFd, buffer, &server_addr);

  if(strcmp(buffer, "NOTFOUND") == 0){
    printf("File Not Found\n");
    exit(1);
  }

  FILE *fptr = fopen("recv.txt", "w");
  if(fptr == NULL){
    error("Can't create file");
    exit(1);
  }

  for(int i = 1; i < MAX_WORDS; i++){
    char message[5];
    message[0] = 'W', message[1] = 'O', message[2] = 'R', message[3] = 'D', message[4] = '0' + i;
    do_write(sockFd, message, &server_addr);
    bzero(buffer, sizeof(buffer));
    do_read(sockFd, buffer, &server_addr);
    printf("Server >> %s\n", buffer);
    if(strcmp(buffer, "END") == 0){
      break;
    }
    fputs(buffer, fptr);
    fputs("\n", fptr);
  }

  fclose(fptr);
  close(sockFd);

  return 0;
}