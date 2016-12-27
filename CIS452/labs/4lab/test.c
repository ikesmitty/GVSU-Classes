/**************************************
 *  * Hayden Miedema
 *   * Sage Heiss
 *    * 
 *     * Lab 04 - CS 452
 *      * ***********************************/

#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void * start(void * arg);
void signalHandler(int);

int dispatch = 1;

int numServiced = 0;

int main() {
  
  int status;
  
  char * inputs[100];
  
  signal (SIGINT, signalHandler);
  int index = 0;
  
  while(dispatch) {
    printf("Type file name: ");
    inputs[index] = malloc(1);
    fgets(inputs[index], 20, stdin);
    if(dispatch == 0){
      break;
    }
    pthread_t worker;
    
    if ((status = pthread_create(&worker, NULL, start, inputs[index])) != 0) {
	    fprintf(stderr, "thread create error %d: %s\n", status, strerror(status));
	    exit(1);
    }
    pthread_detach(worker);
    index++;
  }
  
  return 0;
}

/**************************************
 *  * Worker method
 *   *************************************/
void * start(void * arg) {
  int randTime = rand() % 10; // could return 0-9
  printf("RANDTIME: %d\n", randTime);
    if(randTime < 2){
        // sleep for 7-10 seconds then print
            int randSleep = (rand() % 4) + 7; // get 7-10
                sleep(randSleep);
                    printf("\nFile accessed from hard drive: %s\n", (char *)arg);
                      } else {
                          // sleep 1 second and then print
                              sleep(1);
                                  printf("\nFile accessed from cache: %s\n", (char *)arg);
                                    }
                                      numServiced++;
                                        free(arg);
                                         return arg;
                                         }
  
                                          /**************************************
                                           * Signal Handler
                                            *************************************/
                                            void signalHandler(int signum){
                                              printf(" Signal Received.\n");
                                                dispatch = 0;
                                                  printf("Number of files Serviced: %d\n", numServiced);
                                                 }
