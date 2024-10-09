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
    // open exist shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);

    // map shared memory to process addr space
    void *ptr = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

    // open named semaphore
    sem_t *sem = sem_open(SEM_NAME, 0);

    // wait semaphore and read data
    sem_wait(sem);
    printf("Reader read from shared memory: %s\n", (char*)ptr);
    sem_post(sem);

    // clean 
    munmap(ptr, SHM_SIZE);
    close(shm_fd);
    sem_close(sem);

    return 0;
}