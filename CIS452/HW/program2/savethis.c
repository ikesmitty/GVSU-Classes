/*******************************************
 * Author:     Isaac Smith
 * Assignment: Program 2 Bid Data Sorting
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

void leafSort (char*);
int compare (const void *, const void *);
void merg ();

typedef struct{
    int trans;
    int custID;
    float amount
}order;
order purch[50];
/*typdef struct{
   int trans, custnum;
   float amount;
}info;
info purch[50];*/
int main(int argc, char *argv[]) {
  
  pid_t leftchild;
  pid_t rightchild;
  pid_t pid;
  
  pid = getpid();
  leftchild = fork();
  printf("arg %s\n", argv[1]);
  if(leftchild < 0){
    printf("leftchild fork failed.\n");
  }

  if(leftchild != 0){
    printf("I am the parent. My PID is: %d\n", pid);
    rightchild = fork();
    if(rightchild == 0){
      printf("I am the rightchild. My PID is %d\n", getpid());
      printf("my parent is: %d\n", pid);
      leafSort(argv[2]);
      printf("right child file1 sorted\n");
      for(int i = 0; i < 50; i++){
         printf("%d %d %.2f\n", purch[i].trans, purch[i].custID, purch[i].amount);
      }
    }
    
  }else if(leftchild == 0){
    printf("I am the leftchild. My PID is: %d\n", getpid());
    printf("my parent is: %d\n", pid);
    leafSort(argv[1]);
    printf("left child file2 sorted\n");
    for(int i = 0; i < 50; i++){
       printf("%d %d %.2f\n", purch[i].trans, purch[i].custID, purch[i].amount);

    }
  }
  return 1;
}

void leafSort ( char *filename) {
   int c;
   FILE *file;
   char line[100];
   int trans;
   int custID;
   float amount;
   //order purch[50];
   sleep(30);
   if((file = fopen(filename, "r")) == NULL) {
     printf("Can't open file.\n");
     return;
   }
   printf("Reading the file.\n");
   c = 0;
   while(fgets(line, sizeof (line), file) != NULL){
      sscanf(line, "%d %d %f", &trans, &custID, &amount); 
      purch[c].trans = trans;
      purch[c].custID = custID;
      purch[c].amount = amount;
      c++;
   }
   qsort(purch, 50, sizeof(order), compare);
   printf("Post sort\n");
   for(int i = 0; i<50; i++){
      //printf("%d %d %.2f\n", purch[i].trans, purch[i].custID, purch[i].amount);
   }

   fclose(file);
   return;
}

void merg(){

}

int compare (const void * a, const void * b) {
   //struct *infoA = (info *)a;
   //struct *infoB = (info *)a;
   order *orderA = (order *)a;
   order *orderB = (order *)b;

  return ( orderA->custID - orderB->custID );
}
