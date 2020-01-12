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
  sockFd = socket(AF_INET, SOCK_DGRAM, 0);

  if(sockFd < 0){
    error("[ERROR] Can't connect to socket");
  }
  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portNo);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  char hostName[N], ipAddr[N];
  printf("Enter HostName : ");
  scanf("%s", hostName);
  printf("%s\n", hostName);
  int len = sizeof(server_addr);
  int p = sendto(sockFd, (const char*)hostName, strlen(hostName) + 1, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(p < 0){
    error("Connection failed");
  }
  int n = recvfrom(sockFd, ipAddr, N, 0, (struct sockaddr*)&server_addr, &len);
  ipAddr[n] = '\0';

  printf("IP Address corresponding to %s is %s\n", hostName, ipAddr);

  return 0;
}