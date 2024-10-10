#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/unix_socket_example"


int main()
{
    int client_sock;
    struct sockaddr_un server_addr;

    // 创建套接字
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    
    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // 连接到服务器
    if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(client_sock);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server.\n");

    // 与用户交互，发送消息给服务器并接收回显消息
    char buffer[1024];
    while(1) {
        printf("Enter a message: ");
        fgets(buffer, sizeof(buffer), stdin);

        // 发送消息给服务器
        if (send(client_sock, buffer, strlen(buffer), 0) == -1) {
            perror("Error sending message to server");
            break;
        }

        // 接收来自服务器的回显消息
        ssize_t received_bytes = recv(client_sock, buffer, sizeof(buffer), 0);
        if (received_bytes <= 0) {
            perror("Error receiving message from server");
            break;
        }
        // 打印回显消息
        printf("Server's response: %.*s", (int)received_bytes, buffer);

    }

    // 关闭套接字
    close(client_sock);

    return 0;
}