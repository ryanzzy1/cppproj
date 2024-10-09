#include <iostream>
// #include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

#define SHM_SIZE 1024   // shm size
#define FILE_PATH "shared_file" // ftok gen key used file path


int main()
{
    // gen shared memory and semaphore key value
    key_t shm_key = ftok(FILE_PATH, 'A');
    key_t sem_key = ftok(FILE_PATH, 'B');

    // create shared memory
    int shm_id =shmget(shm_key, SHM_SIZE, 0666 | IPC_CREAT);

    // attach shm to process address space
    char *shm_ptr = (char*) shmat(shm_id, NULL, 0);

    // create semaphore and only one semaphore
    int sem_id = semget(sem_key, 1, 0666 | IPC_CREAT);

    //wait semaphore
    //     struct sembuf
    // {
    //   unsigned short int sem_num;	/* semaphore number */
    //   short int sem_op;		/* semaphore operation */
    //   short int sem_flg;		/* operation flag */
    // };
    struct sembuf sem_op = {0, -1, 0};  // p operation
    semop(sem_id, &sem_op, 1);

    // write data to memory
    strcpy(shm_ptr, "Hello from writer process!");
    printf("Writer wrote to shared memory: %s\n", shm_ptr);


    // release semaphore
    sem_op.sem_op = 1;  // v operation
    
    semop(sem_id, &sem_op, 1);

    // detach shm
    shmdt(shm_ptr);

    return 0;
}