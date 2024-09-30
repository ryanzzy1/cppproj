#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string>
#include <cstring>
#include <cassert>

using namespace std;

int main()
{
    // create pipe
    int pipefd[2]={0};
    int n = pipe(pipefd);

    // subprocess create
    pid_t id = fork();
    if (id == 0)
    {
        // subprocess write into cache
        close(pipefd[0]);
        std::string message = "This is subprocess, send message ...";
        int count = 0;
        char send_buffer[1024];
        while(true)
        {
            if (count == 5)
                break;
            // construct a dynamic string
            snprintf(send_buffer, sizeof(send_buffer), "%s[%d]:%d", message.c_str(), getpid(), count++);
            // write into buffer
            write(pipefd[1],send_buffer, strlen(send_buffer));
            sleep(1);
        }
        close(pipefd[1]);
        exit(0);
    }

    // Father process
    close(pipefd[1]);
    char buffer[1024];
    for (int i = 0; i < 3; i++)
    {
        ssize_t s = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (s > 0)
        {
            buffer[s] = 0;
            cout << "It's Father process " << getpid() << "# " << buffer << endl;
        }
    }
    close(pipefd[0]);
    int status = 0;
    pid_t res = waitpid(id, &status, 0);
    assert(res > 0);
    (void) res;
    printf("Received signal: %d\n", status & 0x7f);

    return 0;
}