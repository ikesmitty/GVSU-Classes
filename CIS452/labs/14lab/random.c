#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int main() {
int randomData = open("/dev/random", O_RDONLY);
char myRandomData[50];
size_t randomDataLen = 0;
while (randomDataLen < sizeof myRandomData)
{
    ssize_t result = read(randomData, myRandomData + randomDataLen, (sizeof myRandomData) - randomDataLen);
    if (result < 0)
    {
        // error, unable to read /dev/random 
    }
    randomDataLen += result;
    printf("%i", result);
}
close(randomData);
}
