#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    // 1. file exist 
    int ret = access("test", F_OK);
    if(ret == -1)
    {
        printf("fifo not exist. create one.\n");

        // 2. create fifo
        ret = mkfifo("test", 0664);

        if(ret == -1) {
            perror("mkfifo");
            exit(0);
        }
    }

    // 3. open fifo as write only format
    int fd = open("test", O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(0);
    }

    // write data
    for (int i = 0; i < 100; i++) {
        char buf[1024];
        sprintf(buf, "hello, %d\n", i);
        printf("write data: %s\n", buf);
        write(fd, buf,strlen(buf));
        sleep(1);
    }
}