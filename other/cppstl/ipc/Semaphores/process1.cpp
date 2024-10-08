#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

int main()
{
    FILE *logFile = fopen("logfile.txt", "a"); // open log file

    // open or create name semaphores
    sem_t *sem = sem_open("/log_semaphore", O_CREAT, 0644, 1);

    // get semaphore
    sem_wait(sem);

    // write log
    fprintf(logFile, "Log message from Process 1\n");
    fflush(logFile);

    // release semaphore
    sem_post(sem);

    // close semaphore and file
    sem_close(sem);
    fclose(logFile);

    return 0;
}
