#include <stdio.h>

#define ITERATIONS 100000

int main()
{
  register int start = 100;
  register int stop = 200;
  register int x =-1;
  for (x = 0; x < ITERATIONS; x++) {
    printf("%8d; Start %10u; Stop %10u; Difference %5d\n", 
	   x, start, stop, (stop-start));
  }
  return 199;
}
