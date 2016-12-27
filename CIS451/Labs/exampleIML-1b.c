//exampleIML-1b.c
//
// Example 1b from the Intel Machine Language lab.
#include <stdio.h>

int main()
{
  int a = 1324;
  int b = 5657;
  int difference = 9876;
  int printf_answer = 2221;

  difference = a - b;
  
  printf_answer = printf("%d + %d = %d\n", a, b, difference);

  return difference;
}