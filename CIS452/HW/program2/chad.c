#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#define NONE 0
#define LEFT 1
#define RIGHT 2

#define READ 0
#define WRITE 1
#define MAX 1024

//struct to store data from one entry in the file
struct record{
  int transactionNum;
  int customerNum;
  double amount;
};

//method declarations
void sigHandler(int);
int calcUnique (char*);
struct record* load(char*,int*);
int cmpCustomerNum(const void*, const void*);
void printStructArray (struct record*,int);
char* buildRecordStr (char*,struct record*);

//global variables for sigHandlers
int readReady = 0;
int sendNext = 1;

int main(int argc, char* argv[])
{
  printf("entering main\n");
  pid_t pid1, pid2;
  int fd_up[2],fd_left[2],fd_right[2];
  printf("filenames\n");
  char fileNames [8][15] = {"file_1.dat\0","file_2.dat\0","file_3.dat\0","file_4.dat\0","file_5.dat\0","file_6.dat\0","file_7.dat\0","file_8.dat\0"};
  
  //Receive number of file from user. Command line or runtime
  int numFiles = -1;
  if(argc>1){
	  printf("checking files\n");
    numFiles = strtol(argv[1],NULL,10);
	printf("number: %d\n", numFiles);
  }
  printf("before whiel\n");
  while(numFiles == 1 || (numFiles & (numFiles - 1)) != 0){
	  printf("inwhile\n");
    printf("Please input a power of 2: ");
    scanf("%d", &numFiles);
  }
  printf("bfore max\n");
  //set up number of levels based on number of files
  int maxLevel = (int) log2(numFiles);
  //set up 'binary' string to ensure unique leaf node indices
  printf("afdt max %d\n", maxLevel);
  char unique [maxLevel];
  printf("aft un\n");
  printf("Number of levels: %d for %d files\n",maxLevel,numFiles);
  int level;
  
  //fork based on number of levels
  for (level = 0; level < maxLevel; level++){
    //set up pipe for first child
    if (pipe(fd_left) < 0) {
      perror ("plumbing problem");
      exit(1);
    }
    if ((pid1 = fork()) < 0){
      perror("Fork failure\n");
      exit(1);
    }else if(pid1 > 0){ // I am the parent
      if (pipe(fd_right) < 0) { //pipe for second child
perror ("plumbing problem");
exit(1);
      }
      if((pid2 = fork()) < 0){ //fork for second child
perror("Fork failure\n");
exit(1);
      } else if(pid2 > 0){ // I am the parent
//close unneeded descriptors
close(fd_left[WRITE]);
close(fd_right[WRITE]);
//register signal handler for ready from children
signal(SIGUSR1, sigHandler);
//register signal handler to wake up
signal(SIGUSR2, sigHandler);
break;
      }else { //I am right child.
//set value for unique index
unique[level]='1';
//save value for writing up
fd_up[WRITE] = fd_right[WRITE];
//close unneeded descriptors
close(fd_left[READ]);
close(fd_right[READ]);
close(fd_left[WRITE]);
//register signal handler to wake up
signal(SIGUSR2, sigHandler);
continue;
      }   
    }//I am left child
    //set value for unique index
    unique[level]='0';
    //save value for writing up
    fd_up[WRITE] = fd_left[WRITE];
    //close unneeded descriptors
    close(fd_left[READ]);
    //register signal handler to wake up
    signal(SIGUSR2, sigHandler);
  }
  
  //our process trees and pipes are initialized. Run the sorting.
  if(level != maxLevel){ //I am a merger/master 
    FILE* fp;
    if(level == 0){ //I am the master
      //open results file to later write to.
      fp = fopen("results.txt", "w");
      if (fp == NULL) {
        perror("Error opening file");
      }
      printf("Master\tLevel %d. I am %d. My parent is %d. My children are %d and %d\n", level,(int)getpid(), (int)getppid(),(int)pid1,(int)pid2);
    } else {
      printf("Merger\tLevel %d. I am %d. My parent is %d. My children are %d and %d\n", level,(int)getpid(), (int)getppid(),(int)pid1,(int)pid2);
    }
    sleep(1);
    
    ssize_t num1, num2; 
    int lastRead = NONE;
    struct record temp1, temp2, min;
    
    //stay alive to continue reading
    while(1) {
      if(readReady >= 2 && sendNext){

//if(level > 0){//I am a merger
//  printf("%d: Waiting for request\n",getpid());
  //pause();//wait until parent sends a request
//}

printf("%d: Reading from pipe\n",getpid());

//read from input
if(lastRead != RIGHT){
  //printf("%d: Reading left\n",getpid());
  num1 = read(fd_left[READ],(void *)&temp1, (size_t)sizeof(struct record));
}
if(lastRead != LEFT){
  //printf("%d: Reading right\n",getpid());
  num2 = read(fd_right[READ],(void *)&temp2, (size_t)sizeof(struct record));
}
//check if there was an error
if(num1 > MAX || num2 > MAX) {
  perror ("pipe read error\n");
  exit(1);
}else if(num1 == 0 || num2 == 0){
  printf("--%d: I am done. There was an error. Goodbye! %d %d\n",(int)getpid(),(int)num1,(int)num2);
  exit(1);
}
//printf("%d: Reading done\n",getpid());

//compare our two customer numbers
if(temp1.customerNum < temp2.customerNum){
  min = temp1;
  lastRead = LEFT;
} else {
  min = temp2;
  lastRead = RIGHT;
}

//send up
if(level > 0){//I am merger
  //write up to parent.
  write(fd_up[WRITE], &min, sizeof(struct record));
  printf("%d: Sent customerNum %d to %d. Choice between %d and %d\n",(int)getpid(),min.customerNum,(int)getppid(),temp1.customerNum,temp2.customerNum);
  //send signal to parent to indicate you are ready
  kill(getppid(),SIGUSR1);
  sendNext = 0;
  printf("%d: ReadReady sent to %d\n",(int)getpid(),(int)getppid());
} else if(level == 0){ //I am the master
  //exit if we have the choice between 2 INT_MAXs
  if(min.customerNum == INT_MAX){
    //exit the program
    fclose(fp);
    printf("\n%d: Master says we must be done sorting! Check out the results in 'results.txt'\n\n",getpid());
    exit(0);
  }
  //output to file
  fprintf(fp,"%d %d %lf\n",min.transactionNum,min.customerNum,min.amount);
  //output for log
  fprintf(stdout,"-%d: %d %d %lf. Choice between %d and %d\n",(int)getpid(),min.transactionNum,min.customerNum,min.amount,temp1.customerNum,temp2.customerNum);
}

//decrement your ready counter
readReady = 1;
//ask for child to send another record
if(lastRead == RIGHT){kill(pid2,SIGUSR2);}
else if(lastRead == LEFT){kill(pid1,SIGUSR2);}
      }
      //We are not ready to read/send. Wait for a signal before trying again
      pause();
      //printf("%d: Awake. ReadReady is %d. SendNext is %d\n",getpid(),readReady, sendNext);
    }//end of stay alive
  } else{ //I am a leaf (sorter)
    //get our unique index to know which file to sort
    int index = calcUnique(unique);
    printf("Leaf%d\tLevel %d. I am %d. My parent is %d.\n", index, level,(int)getpid(), (int)getppid());
    //sleep to clean up initial output
    sleep(1);
    
    int numRecords;
    struct record *records;
    
    //set up dummy record for sending when we are out of records.
    struct record dummy;
    dummy.transactionNum = 0;
    dummy.customerNum = INT_MAX;
    dummy.amount = 0;
    
    //obtain records and sort file
    printf("%d: Loading structs from file\n",(int)getpid());
    records = load(fileNames[index], &numRecords);
    printf("%d: Sorting structs\n",(int)getpid());
    qsort(records, numRecords, sizeof(struct record), cmpCustomerNum);
    printf("%d: Done sorting structs\n",(int)getpid());
    //printStructArray(records,numRecords);
    
    int freed = 0;
    int count = 0;
    //stay alive sending one record at a time
    while(1){
      if(count >= numRecords){ //we are out of records
if(!freed){ //Free records once we are just sending dummy records. Only free once.
  free(records);
  freed = 1;
}
//write up to parent
write(fd_up[WRITE], &dummy, sizeof(struct record));
      } else { //we have more records
//write up to parent
write(fd_up[WRITE], &records[count], sizeof(struct record));
printf("%d: Sent customerNum %d to %d\n",(int)getpid(),records[count].customerNum,(int)getppid());
      }
      count++;
      //send signal to parent to indicate you are ready
      kill(getppid(),SIGUSR1);
      printf("%d: ReadReady sent to %d\n",(int)getpid(),(int)getppid());
      //pause until parent asks for another
      pause();
    }//end stay alive for sorter
  }
  return(0);
}

struct record* load(char* fileName, int* pNumRecords){
  char line[MAX];
  char* token;
  int count = 0;
  struct record *records;
  struct record temp;
  
  records = (struct record *)malloc(sizeof(struct record) * (count+1));
  
  FILE* file;
  //printf("Opening %s\n",fileName);
  file = fopen(fileName, "r");
  
  while(fgets(line, MAX, file) != NULL) {
    //add an element to the array
    records = (struct record *)realloc(records, sizeof(struct record) * (count+1));
    
    token = strtok(line," ");
    temp.transactionNum = atoi(token);
    
    token = strtok(NULL," ");
    temp.customerNum = atoi(token);
    
    token = strtok(NULL," ");
    temp.amount = atof(token);
    
    //copy temp to the array
    memcpy(&records[count],&temp,sizeof(struct record));
    
    count++;
  }
  fclose(file);
  *pNumRecords = count;
  return records;
}

int cmpCustomerNum(const void *a, const void *b){ 
  //cast pointer type
  struct record *ia = (struct record *)a;
  struct record *ib = (struct record *)b;
  
  return ia->customerNum - ib->customerNum;  
}

int calcUnique(char* binary){
  int mult = 1;
  int sum = 0;
  int i;
  for(i=0;i<strlen(binary);i++){
    if(binary[i]=='1'){sum+=mult;}
    mult*=2;
  }
  return sum;
}

void printStructArray(struct record *records, int length){
  printf("Struct Array (Length %d):\n",length);
  int i;
  for(i=0;i<length;i++){
    printf("%d\t%d\t%lf\n",records[i].transactionNum,records[i].customerNum,records[i].amount);
  }
}

void sigHandler(int sigNum)
{
  if(sigNum == SIGUSR1){
    readReady++;
  }else if(sigNum == SIGUSR2){
    sendNext = 1;
    //do nothing. Wake up the pause.
  }
}
