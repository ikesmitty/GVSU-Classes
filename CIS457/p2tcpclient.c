#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv){

	char IPAddress[100];
	char file_name[500];
	int portNumber;
	FILE *fout;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
	//Checks if there was an error creating the socket
	if(sockfd < 0){
		printf("There was an error\n");
		return 1;
	}

	//Reads the IP Address
	printf("Please enter the IP Address of the server you wish to connect to: ");
	scanf("%s", &IPAddress);	

	//Reads the Port Number
	printf("Please enter the port number: ");
	scanf("%d", &portNumber);

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(portNumber);
	serveraddr.sin_addr.s_addr = inet_addr(IPAddress);

	
	int errorCheck = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr_in));
	
	//Checks if there was an error connecting to the socket
	if (errorCheck < 0){
		printf("There was an error connecting\n");
		return 1;
	}

	//Reads the file name someone wishes to download
	printf("Enter the file name: ");
	scanf("%s", &file_name);	

	send (sockfd, file_name, strlen(file_name), 0);

	char file_rename[500];

	int n = recv(sockfd, file_rename, 499, 0);

	if(n < 0){
		printf("The requested file does not exist\n");
		return 0;
	}
	else if(n > 0){
		char copied[500] = "copied_";
		strcat(copied,file_name);

		fout = fopen(copied, "w");
		while(n > 0){
			int write = fwrite(file_rename, 1, n, fout);
			n = recv(sockfd, file_rename, 499, 0);
		}
		printf("\nThe requested file is in the idsk now.\n\n");
		fclose(fout);
	}
	
	close(sockfd);
	return 0;
}

