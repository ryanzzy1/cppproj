#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

 
int main(int argc, char *argv[])
{
    int x;
    scanf("%d", &x);
    printf("main process access variable:%d, (pid:%d)\n",x, (int)getpid());
    int rc = fork();
    if(rc < 0){
        fprintf(stderr, "fork failed\n");
        exit(1);
    }else if (rc == 0) {
       int x = 5;
        
        printf("child prcoess access variable:%d, (pid:%d)\n",x, (int)getpid());
    }else{
        // int wc = wait(NULL);
        x = 8;
        printf("hello, I am parent of %d, variable:%d, (pid:%d)\n", rc, x, (int)getpid());
    }
    return 0;
}