#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
// Using a timeout in c.
// In this case in a client.
int main(int argc, char** argv){

  int sockfd = socket(AF_INET,SOCK_STREAM,0);
  if(sockfd<0){
    printf("There was an error creating the socket\n");
    return 1;
  }
  // How long do we wait...
  struct timeval to;
  to.tv_sec=5; 
  to.tv_usec=0;

  struct sockaddr_in serveraddr;
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_port=htons(9876);
  serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
  // Setting the option on the socket
  // to wait for a particular amount of time
  setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&to,sizeof(to));

  int e = connect(sockfd,(struct sockaddr*)&serveraddr,
		  sizeof(struct sockaddr_in));
  if(e<0){
    printf("There was an error with connecting\n");
    return 1;
  }

  printf("Enter a line: ");
  char line[5000];
  fgets(line,5000,stdin);
  send(sockfd,line,strlen(line),0);
  char line2[5000];
  int n = recv(sockfd,line2,5000,0);
  if(n<0){
    printf("Sorry, had a problem receiving\n");
    return 1;
  }
  printf("Got from server: %s\n",line2);

  return 0;
}

