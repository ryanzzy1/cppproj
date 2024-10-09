// posix shm need -lrt -lpthread when compiling this code.

#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#define SHM_NAME "/example_shm"
#define SHM_SIZE 4096
#define SEM_NAME "/example_sem"

int main()
{
    // create or open shm 
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SHM_SIZE);

    // map shm to process addr space
    void *ptr = mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

    // create or open named semaphore , the initial value is 1
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    // wait semaphore, write data
    sem_wait(sem);

    const char *message = "Hello from writer process!";
    snprintf((char *)ptr, size_t(message),"%s", message);
    printf("Writer wrote to shared memory: %s\n", message);

    sem_post(sem);  // release semaphore


    // clean
    munmap(ptr, SHM_SIZE);
    close(shm_fd);
    sem_close(sem);

    return 0;
}