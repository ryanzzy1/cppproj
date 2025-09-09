#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cerrno>

int main()
{
    // 1. 创建套接字
    int server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        perror("socket creation failed");
        return -1;
    }

    // 2. 绑定地址
    struct sockaddr_un server_addr {};
    server_addr.sun_family = AF_UNIX;
    // 使用文件系统路径名作为地址
    strncpy(server_addr.sun_path, "/tmp/example_socket", sizeof(server_addr.sun_path) - 1);

    // 防止程序意外退出后地址被占用
    unlink(server_addr.sun_path);

    if (bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(server_sockfd);
        return 1;
    }

    // 3. 监听连接
    if (listen(server_sockfd, 5) == -1) {
        perror("listen failed");
        close(server_sockfd);
        return 1;
    }

    printf("Server is listening on %s...\n", server_addr.sun_path);

    // 4. 接受连接
    struct sockaddr_un client_addr {};
    socklen_t client_len = sizeof(client_addr);
    int client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);
    if (client_sockfd == -1) {
        perror("accept failed");
        close(server_sockfd);
        return 1;
    }
    printf("client connected\n");

    // 5. 读写数据
    char buffer[256];
    ssize_t num_bytes = read(client_sockfd, buffer, sizeof(buffer) - 1);
    if (num_bytes == -1) {
        perror("read failed");
    } else {
        buffer[num_bytes] = '\0'; // 添加字符串结束符
        printf("Received from client: %s\n", buffer);

        // 回复客户端
        const char* response = "Hello from server!";
        if (write(client_sockfd, response, strlen(response)) == -1) {
            perror("write failed");
        }
    }

    // 6. 关闭连接
    close(client_sockfd);
    close(server_sockfd);

    // 可选，删除套接字文件
    unlink(server_addr.sun_path);

    return 0;
}