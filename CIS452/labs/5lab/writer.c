/************************************
* Authors:    Isaac Smith
*             Bob Heine
*
* Assignment: Readers and Writers
*             Lab 5
*
* Date:       9/29/2016 
************************************/


#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/ipc.h> 
#include <sys/shm.h>

#define FOO 4096

int main () 
{ 
   int shmId; 
   char *shmPtr;
   struct shmid_ds buf;
   int segsize;
   int turn = 0;
   int writerdone = 0;
   int readerdone = 0;

   //while(!writerdone || !readerdone){
   //   while(!writerdone && !turn && !readerdone){
   //      ;
   //   }
   //   turn = !turn;
   //}

   if ((shmId = shmget (IPC_PRIVATE, FOO, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) { 
      perror ("i can't get no..\n"); 
      exit (1); 
   } 
   if ((shmPtr = shmat (shmId, 0, 0)) == (void*) -1) { 
      perror ("can't attach\n"); 
      exit (1); 
   }

   if(ftok("key.txt", shmId) == -1){
      printf("dead");
      exit(0);
   }
   printf ("value a: %lu\t value b: %lu\n", (unsigned long) shmPtr, (unsigned long) shmPtr + FOO);
   shmctl(shmId, IPC_STAT, &buf);
   segsize = buf.shm_segsz;
   printf("Shared memory size: %d\n", segsize);
   if (shmdt (shmPtr) < 0) { 
      perror ("just can't let go\n"); 
      exit (1); 
   } 
   if (shmctl (shmId, IPC_RMID, 0) < 0) { 
      perror ("can't deallocate\n"); 
      exit(1); 
   }

   return 0; 
}
