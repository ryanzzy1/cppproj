#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/wait.h>
#include <sys/stat.h>
 
int main(int argc, char *argv[])
{
    printf("helloworld (pid:%d)\n", (int)getpid());
    int rc = fork();
    if(rc < 0){
        fprintf(stderr, "fork failed\n");
        exit(1);
    }else if (rc == 0) {
       // printf("hello, I am child (pid:%d)\n", (int)getpid());
        close(STDOUT_FILENO);
        open("./os_test.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU); 
        
        char *myargs[3];
        myargs[0] = strdup("wc");
        myargs[1] = strdup("os_test.c");
        myargs[2] = NULL;
        execvp(myargs[0], myargs);
        printf("this shouldn't print out");
    }else{
        int wc = wait(NULL);
        // printf("hello, I am parent of %d (wc:%d) (pid:%d)\n", rc, wc, (int)getpid());
    }
    return 0;
}