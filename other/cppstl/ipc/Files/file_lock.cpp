#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    const char *file = "./ipc_file";
    int fd = open(file, O_RDWR | O_CREAT, 0666);

    // set file lock
    struct flock fl;
    
    fl.l_type = F_WRLCK;    // set write lock
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0; // lock the whole file
    if(fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("Error locking file");
        return -1;
    }

    write(fd, "Hello from Process A\n", 22);

    // release file lock
    fl.l_type = F_ULOCK;
    fcntl(fd, F_SETLK, &fl);
    close(fd);

    return 0;
}