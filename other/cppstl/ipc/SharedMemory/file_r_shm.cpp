#include <iostream>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

int main()
{
    const char* filename = "shared_file";
    const size_t size = 4096;

    // open shared file
    int fd = open(filename, O_RDONLY);

    // map file to memory
    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // open named semaphore
    sem_t *sem = sem_open("/mysemaphore", O_CREAT, 0666, 1);

    // wait semaphore, read data
    sem_wait(sem);
    printf("Read from shared memory :%s\n", (char*)addr);
    sem_post(sem);

    // clean
    munmap(addr, size);
    close(fd);
    sem_close(sem);

    return 0;
}