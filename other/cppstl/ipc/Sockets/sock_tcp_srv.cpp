#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_In address;



int main()
{
    int server_fd, new_socket;
    
    struct sockaddr_in address;

    int addrlen = sizeof(address);

    char buffer[1024] = {0};

    // create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // define sokcet address 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // bind socket to address and port
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // listen socket
    listen(server_fd, 3);

    while(1) {
        // accept client connection
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        // read client data
        read(new_socket, buffer, 1024);
        printf("Message from client: %s\n", buffer);

        // close client connection
        close(new_socket);
    }

    // close server socket
    
    close(server_fd);

    return 0;
}