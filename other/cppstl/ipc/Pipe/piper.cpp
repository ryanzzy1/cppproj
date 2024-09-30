#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// read data from fifo 
int main()
{
    // 1. open fifo
    int fd = open("test", O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(0);
    }

    // read data
    while(true) {
        char buf[1024] = {0};
        int len = read(fd, buf, sizeof(buf));
        if (len == 0) {
            printf("write side disconnect\n");
            break;
        }
        printf("receive buf : %s\n", buf);
    }

    close(fd);

    return 0;
}