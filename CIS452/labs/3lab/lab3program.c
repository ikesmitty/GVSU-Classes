/************************************************
 * Title:   Lab 3 Communicating Processes Program
 * Authors: Isaac Smith
 *          Bob Heine
 * Date:    9-19-2016
 * *********************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void sigHandler (int);
pid_t pid;

int main () {
    int random1, random2;
    signal (SIGINT, sigHandler);
    signal (SIGUSR1, sigHandler);
    signal (SIGUSR2, sigHandler);
    if ((pid = fork()) < 0) {
        perror ("Failed fork");
        exit (1);
    } else if (!pid) {
        //Child loop
        while(1){
            printf("waiting...\t");
            random1 = rand() % 10;
            random2 = rand() % 2;
            sleep(random1);
            if (random2 == 0) {
                kill(getpid(), SIGUSR1);
            } else {
                kill(getpid(), SIGUSR2);
            }
        }
        printf("exiting child.\n");
    }
    //parent stuff
    else {
        signal (SIGINT, sigHandler);
        while(1) {
            pause();
        }
    }
    return 0;
}

void sigHandler (int sigNum) {
    if (sigNum == SIGUSR1) {
        printf("received a SIGUSR1 signal\n");
    } else if (sigNum == SIGUSR2) {
        printf("received a SIGUSR2 signal\n");
    } else if (pid){
        printf("^C received\n");
        printf("Strike three, you're out of here..\n");
        exit(0);
    } else {
	exit(0);
    }
}
