/*	Authors: Taylor Euwema, Matthew Hoffman, & Isaac Smith
*	Project 1 - TCPClient
*/
 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv){

	char c, IPAddress[100], file_name[500];
	int option, portNumber, loop = 1, tryAgain = 0;
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
	while(loop){
		//Reads the file name someone wishes to download
		printf("Enter the file name: ");
		scanf("%s", &file_name);			

		//Sends the file name to the socket
		send (sockfd, file_name, strlen(file_name), 0);
		char file_rename[500];

		int n = recv(sockfd, file_rename, 499, 0);
		
		fprintf(stderr, "N: %d\n", n);
		//Checks if the file doesn't exist
		if(n < 0){
			fprintf(stderr, "Problem retrieving the file. Exiting...\n");
			return 1;
		}
		//Checks if the file doesn't exist
		else if(n == 0){
			printf("The requested file does not exist\n");
		}
		//The file exists
		else{
			
			//Put the new name into an array
			char copied[500] = "copied_";
			strcat(copied,file_name);	

			//Create the file to write to 
			fout = fopen(copied, "w");

			//Loop until we have read all the data
			while(n > 0){
				int write = fwrite(file_rename, 1, n, fout);
				n = recv(sockfd, file_rename, 499, 0);
			}
			printf("\nThe requested file is in the disk now.\n\n");
			fclose(fout);
		}
		
 		tryAgain = 0;
		
		//This is used to check if the user would like to disconnect 
		do{
			if(tryAgain != 0){
				printf("\nValue was not a 0 or 1. Please try again.");
			}

			printf("\nPlease enter 0 to disconnect. Enter 1 to retrieve another file.");
			tryAgain = 1;	
		} while(((scanf("%d%c", &option, &c) != 2 || c != '\n') && clean_stdin()) || option < 0 || option > 1);

		if(option == 0){
			loop = 0;
			close(sockfd);
		}
			
	}
	return 0;
}

/*This is used to make sure we wait for a user input if we need
 *to loop back and ask the user for an input again 		*/ 
int clean_stdin(){
	while(getchar() != '\n');
	return 1;
}

