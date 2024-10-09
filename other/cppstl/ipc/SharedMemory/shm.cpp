#include <sys/mman.h>
#include <iostream>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
// #include <sys/types.h>
#include <sys/wait.h> // wait(NULL); 

// void* shared_memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS);

int main()
{
    // 创建有名信号量
    sem_t *sem = sem_open("/mysemaphore", O_CREAT, 0666, 1);

    // 创建匿名共享内存
    int *counter = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *counter = 0;

    pid_t pid = fork();
    if (pid == 0) {
        // child process
        for(int i = 0; i < 10; ++i) {
            sem_wait(sem); // wait 信号量

            (*counter)++;   // counter increase
            
            printf("Child process increments counter to %d\n", *counter);
            
            sem_post(sem);  // release semaphore
            
            sleep(1);       // work load 
        }

        exit(0);
    } else {
        // parent process

        for (int i = 0; i < 10; ++i) {
            sem_wait(sem);

            printf("Parent process reads counter as %d\n", *counter);

            sem_post(sem);

            sleep(1);
        }

        // wait child process finish

        wait(NULL);

        sem_close(sem);

        sem_unlink("/mysemaphore");

        munmap(counter, sizeof(int));
    }

    return 0;
}   