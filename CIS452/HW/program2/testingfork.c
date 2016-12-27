/*******************************************
 * Author:     Isaac Smith
 * Assignment: Program 2 Big Data Sorting
 * 	       process management system
 * 	       calls and InterProcess Comm.
 * Due Date:   10/6/2016
 * ****************************************/

#define _GNU_SOURCE
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define READ 0
#define WRITE 1
void sendNextSig (int);
void readSortSig1 (int);
void readSortSig2 (int);
void readSortSig3 (int);
void readSortSig4 (int);
void readMergeSig5 (int);
void readMergeSig6 (int);
void leafSort (char*);
int compare (const void *, const void *);
void mergerprocessleft (char*, char*);
void mergerprocessright (char*, char*);

typedef struct{
    int trans;
    int custID;
    float amount;
}order;
order purch[50];
order merge[500];

//Variables for the sighandler
int sendNext = 1;
int readReady1 = 0;
int readReady2 = 0;
int readReady3 = 0;
int readReady4 = 0;
int readReady5 = 0;
int readReady6 = 0;

int main(int argc, char *argv[]) {
  
  pid_t leftmerge;
  pid_t rightmerge;
  pid_t pid;
  int fd1[2], fd2[2];
  
  if(pipe(fd1) < 0){
     perror ("piping problem");
     exit(1);
  }
  if(pipe(fd2) < 0){
     perror ("piping problem");
     exit(1);
  }
  
  pid = getpid();
  leftmerge = fork();
  //Fork for left merge
  if(leftmerge < 0){
    printf("leftmerge fork failed.\n");
  }
  
  //Parent statement then fork a right merge
  if(leftmerge != 0){
    printf("I am the parent. My PID is: %d\n", pid);
    close(fd1[WRITE]);
    close(fd2[WRITE]);

    rightmerge = fork();

    //If rightmerge sort call merge process
    if(rightmerge == 0){
      printf("I am the rightmerge. My PID is %d\n", getpid());
      printf("my parent is: %d\n", pid);
      mergerprocessright(argv[3], argv[4]);
      while(1){
         break;
      }
    }
  //If leftmerge call mergerprocesses
  }else if(leftmerge == 0){
    printf("I am the leftmerge. My PID is: %d\n", getpid());
    printf("my parent is: %d\n", pid);
    mergerprocessleft(argv[1], argv[2]);
    close(fd2[READ]);
    while(1){
       break;
    }
  }
  return 1;
}

//This will create array of structs from the file then
//sort using qsort
void leafSort ( char *filename) {
   int c;
   FILE *file;
   char line[100];
   int trans;
   int custID;
   float amount;
   //order purch[50];

   //printf("This is the file being sorted: %s\n", filename);

   if((file = fopen(filename, "r")) == NULL) {
     printf("Can't open file.\n");
     return;
   }
   printf("Reading the file.\n");
   c = 0;
   //Grabbing data from file and inserting into the struct array
   while(fgets(line, sizeof (line), file) != NULL){
      sscanf(line, "%d %d %f", &trans, &custID, &amount); 
      purch[c].trans = trans;
      purch[c].custID = custID;
      purch[c].amount = amount;
      c++;
   }
   //Sort my data
   qsort(purch, 50, sizeof(order), compare);
   //printf("Post sort\n");
   for(int i = 0; i<50; i++){
      //printf("%d %d %.2f\n", purch[i].trans, purch[i].custID, purch[i].amount);
   }

   fclose(file);
   return;
}

void mergerprocessleft(char *filename1, char *filename2){
    pid_t leftsort;
    pid_t rightsort;
    pid_t pid;
    int fd3[2], fd4[2];

    if(pipe(fd3) < 0){
       perror ("piping problem");
       exit(1);
    }
    if(pipe(fd4) < 0){
       perror ("piping problem");
       exit(1);
    }

    pid = getpid();
    leftsort = fork();
    //Fork for left sort
    if(leftsort < 0){
        printf("leftsort fork failed.\n");
    }

    //Parent statement then fork a right sort
    if(leftsort != 0){
        printf("I am the parent merger. My PID is: %d\n", pid);
        rightsort = fork();
        close(fd3[WRITE]);
        close(fd4[WRITE]);
        //Need to do something here for reading from parent sort
        //If rightsort sort call leafsort
        if(rightsort == 0){
            printf("I am the rightsort. My PID is %d\n", getpid());
            printf("my parent is: %d\n", pid);
            leafSort(filename2);
            close(fd4[READ]);
         }
        //If leftsort call leafsort
    }else if(leftsort == 0){
        printf("I am the leftsort. My PID is: %d\n", getpid());
        printf("my parent is: %d\n", pid);
        leafSort(filename1);
        close(fd3[READ]);
    }
}

void mergerprocessright(char *filename1, char *filename2){
    pid_t leftsort;
    pid_t rightsort;
    pid_t pid;
    int fd5[2], fd6[2];
    size_t num5, num6;
	signal(SIGUSR3,readSortSig3);
	signal(SIGUSR4,readSortSig4);

    if(pipe(fd5) < 0){
        perror ("piping problem");
        exit(1);
    }
    if(pipe(fd6) < 0){
        perror ("piping problem");
        exit(1);
    }

    pid = getpid();
    leftsort = fork();
    //Fork for left sort
    if(leftsort < 0){
        printf("leftsort fork failed.\n");
    }

    //Parent statement then fork a right sort
    if(leftsort != 0){
        printf("I am the parent merger. My PID is: %d\n", pid);
        rightsort = fork();
        close(fd5[WRITE]);
        close(fd6[WRITE]);
        //struct order temp1;
        //struct order temp2;
		order temp1[100];
		order temp2[100];
        int c = 0;
        //Need to do something here for reading from parent sort
        while (readReady3 == 1 && readReady4 == 1){
            num5 = read(fd5[READ],(void *)&temp1, (size_t)sizeof(temp1));
            num6 = read(fd6[READ],(void *)&temp2, (size_t)sizeof(temp2));
			printf("This is temp1 id: %d\n", temp1->custID);
			printf("This is temp2 id: %d\n", temp2->custID);
            /*if(temp1->custID < temp2->custID){
                merge[c].trans = temp1->trans;
                merge[c].custID = temp1->custID;
                merge[c].amount = temp1->amount;
            }else if(temp1->custID > temp2->custID){
                merge[c].trans = temp2->trans;
                merge[c].custID = temp2->custID;
                merge[c].custID = temp2->custID;
            }else if(temp1->custID == temp2->custID){
                merge[c].trans = temp1->trans;
                merge[c].custID = temp1->custID;
                merge[c].amount = temp1->amount;
                merge[c+1].trans = temp2->trans;
                merge[c+1].custID = temp2->custID;
                merge[c+1].amount = temp2->amount;
                c++;
            }*/
            c++;
        }
		for(int x = 0; x < 50; x++){
			//printf("Merged: %d %d %.2f\n", merge[x].trans, merge[x].custID, merge[x].amount);
		}
        //If rightsort sort call leafsort
        if(rightsort == 0){
            printf("I am the rightsort. My PID is %d\n", getpid());
            printf("my parent is: %d\n", pid);
            leafSort(filename2);
			kill(pid,SIGUSR4);
            close(fd6[READ]);
            for(int j = 0; j < 50; j++){
				printf("sorted passed: %d %d %.2f\n", purch[j].trans,purch[j].custID,purch[j].amount);
                write(fd6[WRITE], &purch[j], sizeof(purch[j]));
            }
        }
        //If leftsort call leafsort
    }else if(leftsort == 0){
        printf("I am the leftsort. My PID is: %d\n", getpid());
        printf("my parent is: %d\n", pid);
        leafSort(filename1);
		kill(pid,SIGUSR3);
        close(fd5[READ]);
        for(int i = 0; i < 50; i++){
			printf("sorted passed: %d %d %.2f\n", purch[i].trans,purch[i].custID,purch[i].amount);
			write(fd5[WRITE], &purch[i], sizeof(purch[i]));
        }
    }
}

//Compare customer numbers and sort
int compare (const void * a, const void * b) {
   //struct *infoA = (info *)a;
   //struct *infoB = (info *)a;
   order *orderA = (order *)a;
   order *orderB = (order *)b;

  return ( orderA->custID - orderB->custID );
}


void sendNextSig (int sigNum)
{
  sendNext = 1;
}

void readSortSig1 (int sigNum)
{
  readReady1 = 1;
  //printf("ready 1 on\n");
}

void readSortSig2 (int sigNum)
{
  readReady2 = 1;
  //printf("ready 2 on\n");
}

void readSortSig3 (int sigNum)
{
  readReady3 = 1;
  //printf("ready 2 on\n");
}

void readSortSig4 (int sigNum)
{
  readReady4 = 1;
  //printf("ready 2 on\n");
}

void readMergeSig5 (int sigNum)
{
  readReady5 = 1;
  //printf("ready 2 on\n");
}

void readMergeSig6 (int sigNum)
{
  readReady6 = 1;
  //printf("ready 2 on\n");
}
