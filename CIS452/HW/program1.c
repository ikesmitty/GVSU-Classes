/******************************************
 * Author: Isaac Smith
 * Assignment: Program 1
 * using the termios library
 * Due Date: Thursday 9/15
 *****************************************/

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#define SIZE 100

void getSecret(char phrase[]){
  static struct termios oldt, newt;
  int i = 0;
  int c;

  //Saving the old setting and copying for resetting
  tcgetattr( STDIN_FILENO, &oldt);
  newt = oldt;

  //setting the flag in the termios struct
  printf("Disabling ECHO.\n");
  newt.c_lflag &= ~(ECHO);

  //setting new bits
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);

  //reading the secret phrase from the console
  printf("Enter secret word/phrase: ");
  while ((c = getchar()) != '\n' && c != EOF && i < SIZE){
    phrase[i++] = c;
  }
  phrase[i] = '\0';

  //resetting old
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int main(void){
  char phrase[SIZE];
  char visible[SIZE];

  getSecret(phrase);
  printf("\nYou entered: %s\n", phrase);

  printf("Default behavior restored.\n");
  printf("Enter visible word/phrase: ");
  scanf("%s", visible);
  return 0;
}
