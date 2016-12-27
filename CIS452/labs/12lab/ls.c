/********************************************
* Lab 12 - ls program
* This program implements ls -i and ls -l
*
* Authors: Isaac Smith and Robert Heine
* *****************************************/
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <grp.h>
#include <time.h>
#include <pwd.h>

// string compare function
int strEquals(char * first, char * second){
  if(strlen(first) != strlen(second)) {
    return 0;
  }
  int i = 0;
  int size = strlen(first);
  while(i < size){
    if(first[i] != second[i]){
      return 0;
    }
    i++;
  }
  return 1;
}

int main(int argc, char * argv[])
{
DIR *dirPtr;
char * name;
struct group * g;
struct stat statBuf;
struct dirent *entryPtr;
int boolean;
char * path;
struct passwd* pw;

switch(argc){ // will be ls
  case 1:
  boolean = 2;
  path = ".";
  break;
  case 2: // possibly  ls [arg] or ls [path]
  if(strEquals(argv[1], "-i") == 1){ // ls -i
      boolean = 1;
      path = ".";
  } else {
      if(strEquals(argv[1], "-l") == 1){ // ls -l
          boolean = 0;
          path = ".";
      } else {

          if(strEquals(argv[1], "-li") == 1 || strEquals(argv[1], "-il") == 1){ // ls -il or -li
              boolean = 3;
              path = ".";
          }else{
              boolean = 2;
              path = argv[1];
          }
      }
  }
  break;
  case 3: // could be ls [arg] [path]  or ls [arg] [arg]
  // Check if second argument is an argument or not.
  if(strEquals(argv[1], "-i") != 1 && strEquals(argv[1], "-l") != 1
    && strEquals(argv[1], "-li") != 1 && strEquals(argv[1], "-il") != 1) {
        printf("Can only handle arguments: -i or -l (not both)\n");
        printf("Format: ./ls [OPTIONAL: argument] [OPTIONAL: path]\n");
        exit(1);
    }
    // this program will not handle both -l and -i at the same time
    if(strEquals(argv[2], "-i") == 1 || strEquals(argv[2], "-l") == 1){
        printf("Can only handle: -i or -l (not both)\n");
        printf("Format: ./ls [OPTIONAL: argument] [OPTIONAL: path]\n");
        exit(1);
    }

    path = argv[2];

    if(strEquals(argv[1], "-i") == 1){
        boolean = 1;
    }

    if(strEquals(argv[1], "-l") == 1){
        boolean = 0;
    }

    if(strEquals(argv[1], "-li") == 1 || strEquals(argv[1], "-il") == 1){
        boolean = 3;
    }
    break;
    default:
    printf("Can only handle arguments: -i or -l (not both)\n");
    printf("Format: ./ls [OPTIONAL: argument] [OPTIONAL: path]\n");
    exit(1);
    }

    if((dirPtr = opendir(path)) <= 0){
        printf("PATH WAS INVALID\n");
        printf("Can only handle arguments: -i or -l (not both)\n");
        printf("Format: ./ls [OPTIONAL: argument] [OPTIONAL: path]\n");
        exit(1);
    }

//file path
char buf[512];

while ((entryPtr = readdir(dirPtr)) != NULL){ // pointer for every file in current directory
  name = entryPtr->d_name;
  switch(boolean){
case 0: // ls -l

  // append to file path
  sprintf(buf, "%s/%s", path, name);
  stat(buf, &statBuf);

  // group information
 if((g = getgrgid(statBuf.st_gid)) == NULL ) {
    fprintf( stderr, "getgrgid: NULL pointer\n" );
    return( EXIT_FAILURE );
  }

  if((pw = getpwuid(statBuf.st_uid)) == NULL ) {
    fprintf( stderr, "getpwuid: no password entry\n" );
    return( EXIT_FAILURE );
}

  // File permissions
  printf( (S_ISDIR(statBuf.st_mode)) ? "d" : "-");
  printf( (statBuf.st_mode & S_IRUSR) ? "r" : "-");
  printf( (statBuf.st_mode & S_IWUSR) ? "w" : "-");
  printf( (statBuf.st_mode & S_IXUSR) ? "x" : "-");
  printf( (statBuf.st_mode & S_IRGRP) ? "r" : "-");
  printf( (statBuf.st_mode & S_IWGRP) ? "w" : "-");
  printf( (statBuf.st_mode & S_IXGRP) ? "x" : "-");
  printf( (statBuf.st_mode & S_IROTH) ? "r" : "-");
  printf( (statBuf.st_mode & S_IWOTH) ? "w" : "-");
  printf( (statBuf.st_mode & S_IXOTH) ? "x" : "-");

  // print other information
  printf(" %i", (int)statBuf.st_nlink);
  printf( " %s", pw->pw_name );
  printf( " %s", g->gr_name );
  printf(" %u", (unsigned)statBuf.st_size);

  //format the time
  char buff[20];
  struct tm * timeinfo;
  timeinfo = localtime(&(statBuf.st_mtime));
  strftime(buff, 20, "%b %d %H:%M", timeinfo);
  printf(" %s", buff);
  printf("\t%-20s\n", entryPtr->d_name);
  break;
case 1: // ls -i
  printf("%d %-15s\n", (int)entryPtr->d_ino, name);
  break;
case 2:
  printf("%-15s\n", name);
  break;
case 3: // ls -il or ls -li
  // append to file path
  sprintf(buf, "%s/%s", path, name);
  stat(buf, &statBuf);

  // Group info
  // Not working on test directory but works for my own?
 if((g = getgrgid(statBuf.st_gid)) == NULL ) {
    fprintf( stderr, "getgrgid: NULL pointer\n" );
    return( EXIT_FAILURE );
  }

  if((pw = getpwuid(statBuf.st_uid)) == NULL ) {
    fprintf( stderr, "getpwuid: no password entry\n" );
    return( EXIT_FAILURE );
  }
  // inode
  printf("%d  ", (int)entryPtr->d_ino);

  // Permissions
  printf( (S_ISDIR(statBuf.st_mode)) ? "d" : "-");
  printf( (statBuf.st_mode & S_IRUSR) ? "r" : "-");
  printf( (statBuf.st_mode & S_IWUSR) ? "w" : "-");
  printf( (statBuf.st_mode & S_IXUSR) ? "x" : "-");
  printf( (statBuf.st_mode & S_IRGRP) ? "r" : "-");
  printf( (statBuf.st_mode & S_IWGRP) ? "w" : "-");
  printf( (statBuf.st_mode & S_IXGRP) ? "x" : "-");
  printf( (statBuf.st_mode & S_IROTH) ? "r" : "-");
  printf( (statBuf.st_mode & S_IWOTH) ? "w" : "-");
  printf( (statBuf.st_mode & S_IXOTH) ? "x" : "-");

  // Other information
  printf(" %i", (int)statBuf.st_nlink);
  printf( " %s", pw->pw_name );
  printf( " %s", g->gr_name );
  printf(" %u", (unsigned)statBuf.st_size);

  //format the time
  timeinfo = localtime(&(statBuf.st_mtime));
  strftime(buff, 20, "%b %d %H:%M", timeinfo);
  printf(" %s", buff);
  printf("\t%-20s\n", entryPtr->d_name);


  }
}
closedir(dirPtr);
return 0;
}
