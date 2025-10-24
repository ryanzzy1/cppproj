#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cerrno>

int main()
{
    // 1. 创建套接字
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creationg failed");
        return 1;
    }

    // 2. 连接服务器
    struct sockaddr_un server_addr {};
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, "/tmp/example_socket", sizeof(server_addr.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect failed");
        close(sockfd);
        return 1;
    }
    printf("Connect to server\n");

    // 3. 发送和接收数据
    const char* message = "Hello from client!";
    if(write(sockfd, message, strlen(message))== -1) {
        perror("write failed");
        close(sockfd);
        return 1;
    }

    char buffer[256];
    ssize_t num_bytes = read(sockfd, buffer, sizeof(buffer) - 1);
    if (num_bytes == -1) {
        perror("read failed");
    } else {
        buffer[num_bytes] = '\0';
        printf("Received from server: %s\n", buffer);
    }

    // 4. 关闭连接
    close(sockfd);
    return 0;
}
