#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    // create socket
    int sockCli = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockCli == -1) {
        perror("socket");
    }

    struct sockaddr_in addrSer; 
    addrSer.sin_family = AF_INET;   // set AF_INET protocol
    addrSer.sin_port = htons(5050);
    addrSer.sin_addr.s_addr = inet_addr("192.168.226.169"); // set ip address
    socklen_t addrlen = sizeof(struct sockaddr);

    char sendbuf[256];  // apply a data send buffer
    char recvbuf[256];  // apply a data receive buffer

    while(1) {
        // send data to server
        printf("Cli:>");
        scanf("%s", sendbuf);
        sendto(sockCli, sendbuf, strlen(sendbuf)+1, 0, (struct sockaddr*)&addrSer, addrlen);

        // receive data from server
        recvfrom(sockCli, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&addrSer, &addrlen);
        printf("Ser:>%s\n", recvbuf);

    }

    return 0;
    
}