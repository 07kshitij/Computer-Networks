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

int max(int a, int b){
  return ((a > b) ? a : b);
}

int main(int argc, char const *argv[]){
  
  int sockFd1, sockFd2, portNo, newsockFd;
  struct sockaddr_in server_addr, client_addr1, client_addr2;

  if(argc < 2){
    error("[ERROR] Pls specify portNo");
  }
  portNo = atoi(argv[1]);
  sockFd1 = socket(AF_INET, SOCK_STREAM, 0);
  sockFd2 = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockFd1 < 0){
    close(sockFd1);
    error("[ERROR] Can't create TCP socket");    
  }
  if(sockFd2 < 0){
    close(sockFd2);
    error("[ERROR] Can't create UDP socket");    
  }

  memset(&server_addr, 0, sizeof(server_addr));
  memset(&client_addr1, 0, sizeof(client_addr1));
  memset(&client_addr2, 0, sizeof(client_addr2));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portNo);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(sockFd1, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    close(sockFd1); close(sockFd2);
    error("[ERROR] Can't bind to TCP socket");
  }

  if(bind(sockFd2, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    close(sockFd1); close(sockFd2);
    error("[ERROR] Can't bind to UDP socket");
  }

  listen(sockFd1, 5);
  fd_set readSocket;

  while(1){
    FD_ZERO(&readSocket);
    FD_SET(sockFd1, &readSocket);
    FD_SET(sockFd2, &readSocket);

    int fdmax = max(sockFd1, sockFd2) + 1;
    int rec = select(fdmax, &readSocket, 0, 0, 0);
    if(rec < 0){
      close(sockFd1); close(sockFd2);
      error("[ERROR] Can't make select call");
    }

    if(FD_ISSET(sockFd1, &readSocket)){
      int len = sizeof(client_addr1);
      printf("\n~~~Connected to TCP Socket~~~\n");
      printf("~~~ Reading from %s : %d ~~~\n", inet_ntoa(client_addr1.sin_addr), ntohs(client_addr1.sin_port));
      newsockFd = accept(sockFd1, (struct sockaddr*)&client_addr1, &len);
      if(newsockFd < 0){
        error("[ERROR] Can't connect to TCP Client");
      }
      char fileName[N], fileContent[N];
      fileName[0] = '\0'; fileContent[0] = '\0';

      int total_size = recv(newsockFd, fileName, N, 0);
      int fd = open(fileName, O_RDONLY);
      FILE *file = fopen(fileName, "r");
      if(file != NULL){
        int r = 0;
        printf("\tFILE NAME : %s\n", fileName);
        while((fgets(fileContent, sizeof(fileContent), file)) != NULL){
          fileContent[strlen(fileContent) - 1] = '\0';
          send(newsockFd, fileContent, sizeof(fileContent), 0);
          printf("\tLine %d %s\n", r, fileContent);
          r++;
        }
        fileContent[0] = '\0';
        send(newsockFd, fileContent, sizeof(fileContent), 0);
        fclose(file);
      }else{
        perror("[ERROR] FileNotFound");
        send(newsockFd, fileContent, sizeof(fileContent), 0);
        break;
      }
    }
    if(FD_ISSET(sockFd2, &readSocket)){
      printf("\n~~~Connected to UDP Socket~~~\n");
      printf("~~~ Reading from %s : %d ~~~\n", inet_ntoa(client_addr2.sin_addr), ntohs(client_addr2.sin_port));
      char hostName[N], ipAddr[N];
      hostName[0] = '\0'; ipAddr[0] = '\0';
      int len = sizeof(client_addr2);
      int total_size = recvfrom(sockFd2, hostName, N, 0, (struct sockaddr*)&client_addr2, &len);
      if(total_size < 0){
        error("[ERROR] Invalid HostName");
      }
      printf(GREEN"\t< Domain : %s >", hostName);
      struct hostent *hostIP;
      hostIP = gethostbyname(hostName);
      if(hostIP == NULL){
        herror("[ERROR] HostName not found");
        break;
      }

      struct in_addr **address_list = (struct in_addr **)hostIP -> h_addr_list;

      for(int i = 0; address_list[i] != NULL; i++){
        if(i){
          strcat(ipAddr, " ... ");
        }
        strcat(ipAddr, inet_ntoa(*address_list[i]));
      }
      printf(BLUE"\t< IP Address : %s >\n", ipAddr);
      sendto(sockFd2, ipAddr, strlen(ipAddr) + 1, 0, (struct sockaddr*)&client_addr2, len);
    }
  }

  close(sockFd1);
  close(sockFd2);

  return 0;
}