// read process

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    const char *file = "./ipc_file";
    int fd = open(file, O_RDWR | O_CREAT, 0666);
    char buf[50];
    read(fd, buf, 20);
    std::cout << "Received from b process: " << buf << std::endl;
    close(fd);

    return 0;
}