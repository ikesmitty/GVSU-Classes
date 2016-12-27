/*******************************************
 * Author:     Isaac Smith
 * Assignment: Program 2 Bid Data Sorting
 * 	       process management system
 * 	       calls and InterProcess Comm.
 * Due Date:   10/6/2016
 * ****************************************/

#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
  
  pid_t leftchild;
  pid_t rightchild;
  pid_t pid;
  
  if(!(argc % 2)) {
    printf("Please enter files in multiples of 2 to sort\n");
    return 0;
  }else if(argc > 9) {
    printf("Please enter in no more than 8 files to sort\n");
  }
  
  pid=getpid(); 
  for(int i = 0; i < 1; i++) {
    leftchild=fork();
    if(leftchild==0){
      printf("Created left child\n");
    }else{
      rightchild=fork();
      if(rightchild==0){
        printf("Created right child\n");
      }
    }
    wait(NULL);
  //for(int i = 1; i < argc; i++) {
  //
  //} 
  }
}
