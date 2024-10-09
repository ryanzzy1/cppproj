#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

#define SHM_SIZE 1024   // shm size
#define FILE_PATH "shared_file" // ftok gen key used file path

int main()
{
    // create or open shm and sem key value
    key_t shm_key = ftok(FILE_PATH, 'A');
    key_t sem_key = ftok(FILE_PATH, 'B');

    // access shared memory
    int shm_id = shmget(shm_key, SHM_SIZE, 0666);

    // attach memory to process address space
    char *shm_ptr = (char*) shmat(shm_id, NULL, 0);

    // get sem
    int sem_id = semget(sem_key, 1, 0666);

    // wait sem
    struct sembuf sem_op = {0, -1, 0};  // p operation

    semop(sem_id, &sem_op, 1);

    // read content from shared memory
    printf("Reader read from shared memory: %s\n", shm_ptr);

    // release sem
    sem_op.sem_op = 1; // v operation
    semop(sem_id, &sem_op, 1);

    // detach shared memory
    shmdt(shm_ptr);

    return 0;
}