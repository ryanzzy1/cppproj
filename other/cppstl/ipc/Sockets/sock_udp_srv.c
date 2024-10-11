#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    // create socket

    int sockSer = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockSer == -1) {
        perror("socket");
    }

    struct sockaddr_in addrSer; // create struct to record address info
    addrSer.sin_family = AF_INET;   // set AF_INET protocol
    addrSer.sin_port = htons(5050); // set port
    addrSer.sin_addr.s_addr = inet_addr("192.168.226.169"); // set ip

    // set socket with ip address 
    socklen_t addrlen = sizeof(struct sockaddr);
    int res = bind(sockSer, (struct sockaddr*)&addrSer, addrlen);
    if (res == -1) {
        perror("bind");
    }

    char sendbuf[256];  // apply a data send buffer
    char recvbuf[256];  // apply a data receive buffer
    printf("sizeof recvbuff: %lu\n", sizeof(recvbuf));
    struct sockaddr_in addrCli;
    while (1)
    {
        recvfrom(sockSer, recvbuf, 256, 0, (struct sockaddr*)&addrCli, &addrlen); // receive from specified ip address
        printf("Cli: >%s\n", recvbuf);

        printf("Ser:>");
        scanf("%s", sendbuf);
        sendto(sockSer, sendbuf, strlen(sendbuf)+1, 0, (struct sockaddr*)&addrCli, addrlen); // send data to client

    }

    return 0;
}