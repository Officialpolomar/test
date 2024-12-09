#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


int create_ipv4_socket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in create_ipv4_address(char* ip, int port)
{
    struct sockaddr_in address;
    address.sin_port = htons(port);
    address.sin_family = AF_INET;

    if(strlen(ip) == 0)
    {
        printf("\nDefault address given\n");
        address.sin_addr.s_addr = INADDR_ANY;

    } 
    else
    {
        inet_pton(AF_INET, ip, &address.sin_addr.s_addr);
    }
    
    return address;
}