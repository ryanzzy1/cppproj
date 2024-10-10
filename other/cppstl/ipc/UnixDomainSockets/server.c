
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/unix_socket_example"


int main(){
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    unlink(SOCKET_PATH);
    // 创建域套接字
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // 绑定套接字到地址
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 监听连接
    if (listen(server_sock, 5) == -1) {
        perror("Error listening for connections");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening for connections... \n");

    // 接受连接
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock == -1) {
        perror("Error acceping connection");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Connection established with a client.\n");

    // 接收和回显消息
    char buffer[1024];
    while(1) {
        // 接收来自客户端的消息
        ssize_t received_bytes = recv(client_sock, buffer, sizeof(buffer), 0);
        if (received_bytes <= 0) {
            perror("Error receiving message");
            break;
        }

        // 打印接收到的消息
        printf("Received from client: %.*s", (int)received_bytes, buffer);

        // 回显消息给客户端
        if (send(client_sock, buffer, received_bytes, 0) == -1) {
            perror("Error sending message back to client");
            break;
        }
    }

    // 关闭套接字
    close(client_sock);
    close(server_sock);
    unlink(SOCKET_PATH);

    return 0;
}