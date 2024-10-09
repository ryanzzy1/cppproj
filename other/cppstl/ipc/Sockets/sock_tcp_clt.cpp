#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_In address;
#include <string.h>


int main()
{
    int sock = 0;

    struct sockaddr_in serv_addr;

    // create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // define server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // connect to server
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // send data to server
    char *message = "Hello from the client";
    send(sock, message, strlen(message), 0);

    // close socket
    close(sock);

    return 0;
}