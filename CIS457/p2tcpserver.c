#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
	FILE *fp;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serveraddr, clientaddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9876);
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	
	//forces a write of all user-space buffered data for the output.
	fflush(stdout);
        
	//Create the file space for the socket and register it as a socket
	bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	//Marks the socket as a passive socket. It will be used to accept 
	//incoming connection requests
	listen(sockfd, 1);

	//Accepts the socket. This extracts the first connection request on the queue of 
	//pending connectings for the listening socket, sockfd, creates a new connected socket
	//and returns a new file descriptor referring to that socket. 
	int length = sizeof(clientaddr);
	int clientsocket = accept(sockfd, (struct sockaddr*)&clientaddr, &length);
	
	if(clientsocket > -1)
	    printf("A client has connected to the server./n");
	char fileName[500];
	char store[500];

	//Returns the length of the fileName
	int r = recv(clientsocket, fileName, 500, 0);

	//Adds a null character to the end of the fileName
	fileName[r] = '\0';

	//Read the file and return a pointer to the file.
	fp = fopen(fileName, "r");

	//Checks if the file exists
	if (fp == NULL){
		printf("\nThe file requested does not exist. Disconnecting\n\n");
		
		//Close the connections
		close(clientsocket);
		close(sockfd);
	}
	else{
		//Loop until the end of file char is found
		while(feof(fp) == 0){

			//Read the file pointed to by fp and store the data in store
			int read = fread(store, 1, 499, fp);
	
			//Sends the data in store to the clientsocket
			send(clientsocket, store, read, 0);
		}
		
		//Close the file pointer
		fclose(fp);
		printf("\nThe requested file has been sent.\n\n");
	}

	//Close the connections
	close(clientsocket);
	close(sockfd);
	return 0;
}

