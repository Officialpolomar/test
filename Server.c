#include "util.h"

#define BUFFER_SIZE 2000

int main()
{
    int server_fd = create_ipv4_socket();
    char* server_ip = "";
    int port = 2000;
    char buffer[BUFFER_SIZE];

    struct sockaddr_in server_address = create_ipv4_address(server_ip, port);
    int server_size = sizeof(server_address);

    int bind_result = bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address));

    if(bind_result >= 0)
    {
        printf("\nbind successful");
    }
    else 
    {
        perror("bind error");
        exit(1);
    }

    int listen_result = listen(server_fd, 3);
    
    if(listen_result >= 0)
    {
        printf("\nListening at port %d", port);
    }
    else 
    {
        printf("\nListening failed");
    }
   
    int client_socket_FD = accept(server_fd, &server_address, (socklen_t*)&server_size);

    if(client_socket_FD >= 0)
    {
        printf("\nconnection accepted\n");
    }
    
    while(1)
    {
        int amount_recieved =  read(client_socket_FD, buffer, BUFFER_SIZE);

        if(amount_recieved >= 0)
        {
            printf("Response was %s\n", buffer);
        }
        if(amount_recieved < 0)
        { 
            break;
        }

    }

    close(client_socket_FD);
    

}