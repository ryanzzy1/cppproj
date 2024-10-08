// write process

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    const char *file = "./ipc_file";
    int fd = open(file, O_RDWR | O_CREAT, 0666);
    write(fd, "Hello from Process A string.\n", 30);
    close(fd);

    return 0;
}