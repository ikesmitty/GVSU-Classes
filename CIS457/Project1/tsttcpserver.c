/*	Authors: Taylor Euwema, Matthew Hoffman, & Isaac Smith
*	Project 1 - TCPServer
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

void func(x){

char fileName[500];
char store[500];
//Returns the length of the fileName
int r = recv(x, fileName, 500, 0);
//Adds a null character to the end of the fileName
fileName[r] = '\0';
//Read the file and return a pointer to the file
//Checks if the file exists
if (fp == NULL) {
printf("\n The file requested does not exist. Disconnecting\n\n");

//Close and clear the connection
close(i);
FD_CLR(i, &sockets);

} else {
    //Loop until the end of file char is found
    while(feof(fp) == 0){
    //Read the file pointed to by pf and store the data in store
    int read = fread(store, 1, 499, fp);

    //Sends the data in store to the clientsocket
                          send(i, store, read, 0);
}

//Close and clear the connection
close(i);
FD_CLR(i, &sockets);
printf("The requested file has been sent");
}


int main(int argc, char **argv){
	FILE *fp;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int portnum;
	fd_set sockets; //Set of sockets
	FD_ZERO(&sockets); //Clears all the sockets
	
	//Reads the port number
	printf("Please enter designated port number:");
	scanf("%d", &portnum);

	struct sockaddr_in serveraddr, clientaddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(portnum);
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	
	//forces a write of all user-space buffered data for the output.
	fflush(stdout);
        
	//Create the file space for the socket and register it as a socket
	bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	//Marks the socket as a passive socket. It will be used to accept 
	//incoming connection requests
	listen(sockfd, 10);
	FD_SET(sockfd,&sockets);

	//Accepts the socket. This extracts the first connection request on the queue of 
	//pending connectings for the listening socket, sockfd, creates a new connected socket
	//and returns a new file descriptor referring to that socket. 
	int length = sizeof(clientaddr);
	while(1){

		fd_set tmp_set = sockets;
		select(FD_SETSIZE, &tmp_set, NULL, NULL, NULL);

		int i;

		//Loop through the set of sockets
		for(i=0; i<FD_SETSIZE; i++){

			//Check if socket is set
			if(FD_ISSET(i, &tmp_set)){

				//Checcks if sockfd is a member of the tmp_set
				if(i==sockfd){
					printf("A client connected\n");
					int clientsocket = accept(sockfd, (struct sockaddr*)&clientaddr, &length);
					FD_SET(clientsocket, &sockets);
	    
				} else {
					pthread(chile, NULL, func, i);
				}
			}
		}
}
