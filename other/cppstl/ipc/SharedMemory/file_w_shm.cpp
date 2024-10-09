#include <iostream>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

int main()
{
    const char* filename = "shared_file";

    const size_t size = 4096;

    // open shared file

    int fd = open(filename, O_RDWR | O_CREAT, 0666);

    // map file to memory

    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // open named semaphore
    sem_t *sem = sem_open("/mysemaphore", O_CREAT, 0666, 1);

    // wait semaphore, write data

    sem_wait(sem);
    strcpy((char*)addr, "Hello from writer process!");
    sem_post(sem);

    // clean
    munmap(addr, size);
    close(fd);
    sem_close(sem);

    return 0;
}