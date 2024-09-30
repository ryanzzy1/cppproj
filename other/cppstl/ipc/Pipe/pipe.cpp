// https://blog.csdn.net/bang___bang_/article/details/132128822
// 


#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

int main()
{
    // 1.create pipe
    int pipefd[2] = {0};    // pipefd[0] reader, pipefd[1] writer
    int n = pipe(pipefd);
    assert(n != -1);
    (void)n;

#ifdef DEBUG
    cout << "pipefd[0]:" << pipefd[0] << endl;  //3
    cout << "pipefd[1]:" << pipefd[1] << endl;  //4
#endif

    // 2.create sub process
    pid_t id = fork();
    assert(id != -1);
    if(id == 0)
    {
        // subprocess
        //3. single direction communication channel, main process write into, sub process read out.
        //3.1 close unnecessary fd of subprocess
        close(pipefd[1]);
        char buffer[1024];
        while(true)
        {
            ssize_t s = read(pipefd[0], buffer, sizeof(buffer)-1);
            if (s > 0)
            {
                buffer[s] = 0; // manual add /0
                cout << "This is subprocess[" << getpid() << "]Father# " << buffer << endl;
            } else {
                cout << s << endl;
            }
            
            sleep(1);
        }

        exit(0);
    }


    // Father process
    // 3. construct a single direction channel
    // 3.1 close subprocess unnecessary fd
    close(pipefd[0]);
    string message = "This is Father process, sending message ...";
    int count = 0;
    char send_buffer[1024];
    while(true)
    {
        // 3.2 construct a string
        snprintf(send_buffer, sizeof(send_buffer), "%s[%d]:%d", message.c_str(), getpid(), count++);

        // 3.3 write
        write(pipefd[1], send_buffer, strlen(send_buffer));

        //3.4 sleep
        sleep(5);
    }
    pid_t res = waitpid(id, nullptr, 0);
    assert(res > 0);
    (void)res;
    close(pipefd[1]);
    return 0;
}