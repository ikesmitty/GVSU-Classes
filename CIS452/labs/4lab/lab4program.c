/***********************************
 * Title: 	Lab4Program
 * Authors: 	Isaac Smith
 * 	    	Bob Heine
 * Description: Simple Blocking 
 *		Multithreaded File
 *		Server
 * Date: 9/29/16
 **********************************/

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SIZE 16

void sighandler(int);
void* worker(void* arg);
int dispatch = 1;
int serviced = 0;

int main() {
	char *filename;
	filename = malloc(SIZE);
	int status;
	signal (SIGINT, sighandler);

	while(dispatch) {
	   printf("Input file string: ");
	   scanf("%s", filename);
	   if(dispatch == 0){
	     break;
	   }
	   pthread_t thread;
	   if ((status = pthread_create(&thread, NULL, worker, filename)) !=0) {
	     printf("creation Error");
	   }
	   pthread_detach(thread);
	   // random will determine which function to call via if statement
	}
	return 0;
}

void sighandler(int signum) {
	printf("\nSignal received\n");
	dispatch = 0;
	printf("\nNumber of files Services: %d\n", serviced);
	exit(0);
}

void* worker(void* arg) {
	char *file = (char *) arg;
	int randnum = rand()%10;
	if(randnum < 2) {
	   printf("\nFile %s not in cache, going to memory\n", file);
	   randnum = (rand()%3)+7;
	   sleep(randnum);
	   printf("\nFile accessed from hdd: %s\n", file);
	} else {
	   sleep(1);
	   printf("\nFile %s found in cache\n", file);
	}
	serviced++;
	return file;
}
