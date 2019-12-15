#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

const int N = 2048;
const int MAX_WORDS = 10;
const int MAX_LEN = 50;

void error(char *message){
  fprintf(stderr, "%s >> ", "ERROR");
  fprintf(stderr, "%s\n", message);
}

bool fileExists(char fileName[], char **contents, int *count){
  FILE *fptr = fopen(fileName, "r");
  if(fptr == NULL){
    return 0;
  }

  for(int i = 0; i < MAX_WORDS; i++){
    fscanf(fptr, "%s", contents[i]);
    (*count)++;
    if(strcmp("END", contents[i]) == 0){
      fclose(fptr);
      return 1;
    }
  }
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
  
  int sockFd, newsockFd, portNo;
  char buffer[N];

  struct sockaddr_in server_addr, client_addr;

  if(argc < 2){
    error("No port provided");
    exit(1);
  }

  portNo = atoi(argv[1]);

  sockFd = socket(AF_INET, SOCK_DGRAM, 0);

  if(sockFd < 0){
    error("Socket can't be opened");
    exit(1);
  }

  bzero((char*) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portNo);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(sockFd, (struct sockaddr*) & server_addr, sizeof(server_addr)) < 0){
    error("Binding Error");
    exit(1);
  }

  newsockFd = sockFd;

  printf("Server Running...\n");

  bzero(buffer, sizeof(buffer));
  do_read(newsockFd, buffer, &server_addr);
  printf("Client >> %s\n", buffer);
  char **fileContents = (char **)malloc(MAX_WORDS * sizeof(char*));
  for(int i = 0; i < MAX_WORDS; i++){
    fileContents[i] = (char*)malloc(MAX_LEN * sizeof(char));
  }
  int len = 0;
  bool fileFound = fileExists(buffer, fileContents, &len);

  if(fileFound){
    do_write(newsockFd, fileContents[0], &server_addr);
    for(int i = 1; i < len; i++){
      bzero(buffer, sizeof(buffer));
      do_read(newsockFd, buffer, &server_addr);
      printf("Client >> %s\n", buffer);
      do_write(newsockFd, fileContents[i], &server_addr);
    }
  }else{
    error("FILE NOTFOUND");
    char message[] = "NOTFOUND";
    do_write(newsockFd, message, &server_addr);
    exit(1);
  }
  close(sockFd);
  close(newsockFd);

  return 0;
}
