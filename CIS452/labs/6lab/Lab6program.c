/*********************************************
* Authors: 	Isaac Smith
*   		Bob Heine
*
* Assignment:	Lab program 6
*		Implement semaphores into
*		sample 1 to control
*		process synchronization
* 
* Due Date: 	10/13/2016
*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define SIZE 16

union semun {
  int val;    // Value for SETVAL
  struct semid_ds *buf;    // Buffer for IPC_STAT, IPC_SET
  unsigned short  *array;  // Array for GETALL, SETALL
  struct seminfo  *__buf;  // Buffer for IPC_INFO
} semun;


int main (int argc, char * argv[]) {
    int status;
    long int i, loop, temp, *shmPtr;
    int shmId;
    pid_t pid;
    
    if(argc > 0){
      loop = atof(argv[1]);
    } else {
      loop = 0;
    }

    if ((shmId = shmget (IPC_PRIVATE, SIZE,
                         IPC_CREAT | S_IRUSR | S_IWUSR)) < 0) {
        perror ("i can't get no..\n");
        exit (1);
    }
    if ((shmPtr = shmat (shmId, 0, 0)) == (void *) -1) {
        perror ("can't attach\n");
        exit (1);
    }

    shmPtr[0] = 0;
    shmPtr[1] = 1;
    
    // Using common key IPC_PRIVATE, create 1 semaphore with chmod values 666
    int semId = semget (IPC_PRIVATE, 1, 0600); // 1 semaphores

    if(semId < 0) {
      printf("Unable to obtain semaphore.\n");
      exit(0);
    }
    
    semun.val = 0; // the intial value to be set by semctl later
    
    struct sembuf ops1[2]; // have two operations
    ops1[0].sem_num = 0; // the 1st semaphore
    ops1[0].sem_op = 0; // wait for 0
    ops1[0].sem_flg = SEM_UNDO; // take off semaphore asynchronous
    ops1[1].sem_num = 0; // the 1st semaphore
    ops1[1].sem_op = 1; // increment semaphore
    ops1[1].sem_flg = SEM_UNDO | IPC_NOWAIT; // take off semaphore asynchronous
    
    struct sembuf ops2[1]; // have one operation
    ops2[0].sem_num = 0; // the 1st semaphore
    ops2[0].sem_op = -1; // the semaphore operation - subtract 1
    ops2[0].sem_flg = SEM_UNDO | IPC_NOWAIT; // take off semaphore asynchronous
    
    // set semval of 0th semaphore in semId to semun.val
    if( semctl(semId, 0, SETVAL, semun) < 0) {
      printf("Unable to initialize 0th semaphore.\n");
      exit(0);
    }

    if (!(pid = fork ())) {
        for (i = 0; i < loop; i++) {
			semop(semId, ops1, 2); // wait and increment
          
			temp = shmPtr[0];
			shmPtr[0] = shmPtr[1];
			shmPtr[1] = temp;
	  
			semop(semId, ops2, 1); // decrement
        }
        if (shmdt (shmPtr) < 0) {
            perror ("just can 't let go\n");
            exit (1);
        }
        exit (0);
    }
    else {
        for (i = 0; i < loop; i++) {
			semop(semId, ops1, 2); // wait and increment
          
			temp = shmPtr[0];
			shmPtr[0] = shmPtr[1];
			shmPtr[1] = temp;
	  
			semop(semId, ops2, 1); // decrement
        }
    }

    wait (&status);
    printf ("values: %li\t%li\n", shmPtr[0], shmPtr[1]);
    
    if( semctl(semId, 0, IPC_RMID) < 0) {
      printf("Unable to remove semaphore.\n");
    }
    printf("Removed semaphore\n");

    if (shmdt (shmPtr) < 0) {
        perror ("just can't let go\n");
        exit (1);
    }
    printf("Removed shared memory pointer\n");
    if (shmctl (shmId, IPC_RMID, 0) < 0) {
        perror ("can't deallocate\n");
        exit (1);
    }
    printf("Deallocated sharedmemory space\n");
    return 0;
}
