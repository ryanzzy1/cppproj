// [原文](https://blog.csdn.net/weixin_42307601/article/details/130667724?spm=1001.2014.3001.5502)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // ip address
#include <arpa/inet.h>  // inet_pton() 原型

#define PORT 8888
#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 1024

// 端口号 TCP中是一个16位的整数，范围0~65535
// 每个端口号与一个唯一的应用程序或服务相关联，用于标识应用程序或服务在网络中的位置，实现网络通信。
// IP地址用于确定，数据发送到哪一个主机上面，而这个主机同时在运行很多程序，为了确保数据能够正确交付
// 给对应的程序，那么就用端口号来进行标识，一个端口号就可以确定唯一的一个进程，并且一个进程也可以拥有多个端口号，但是一个端口号只能和一个进程绑定

// struct sockaddr_in; // ipv4 
// struct sockaddr_in6; // ipv6
/*
//create socket 
int main()
{
    int sockid = 0;
    sockid = socket(AF_INET, SOCK_STREAM, 0);
    if (sockid < 0)
    {
        printf("Socket create fail\n");
    }
    else 
    {
        printf("Socket create success and socket id is : %d\n", sockid);
    }

    return 0;
}*/

/*
int main()
{
    const char *ip = "127.0.0.1";
    struct in_addr addr;

    if (inet_pton(AF_INET, ip, &addr) <= 0) {
        printf("Failed to convert IP address\n");
        return -1;
    }

    printf("IP address : 0x%x\n", addr.s_addr);

    return 0;
}*/

int main()
{
    int sockfd, client_sockfd, bytes_received;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_addrlen = sizeof(client_addr);

    char buffer[BUFFER_SIZE];
    char *response = "Hello, client!";

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 配置srv地址和端口信息
    bzero(&serv_addr, sizeof(serv_addr)); // 清空内容
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // 绑定地址结构到套接字上
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 设置套接字为监听状态
    if (listen(sockfd, MAX_CONNECTIONS) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("server listening on port %d\n", PORT);

    while(1) {
        // 接收新的客户端请求
        client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen);
        if (client_sockfd == -1) {
            perror("accept");
            continue;
        }

        // 接收客户端数据
        bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            perror("recv");
            close(client_sockfd);
            continue;
        }

        printf("Received %d bytes from client: %s\n", bytes_received, buffer);

        // 向客户端发送数据
        if(send(client_sockfd, response, strlen(response), 0) == -1) {
            perror("send");
            close(client_sockfd);
            continue;
        }

        printf("Sent response to client: %s\n", response);

        // 关闭当前套接字
        close(client_sockfd);
        printf("Client connection closed\n");
        
    }

    // 关闭服务端套接字
    close(sockfd);


    return 0;
}
