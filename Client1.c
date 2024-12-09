#include "util.h"
#define BUFFER_SIZE 1024

int main()
{
char message[BUFFER_SIZE];
int socketFD = create_ipv4_socket();
 printf("%d\n", socketFD);
 char* ip_address = "127.0.0.1";

 struct sockaddr_in sock_address = create_ipv4_address(ip_address, 2000);


 int result = connect(socketFD, (struct sockaddr*)&sock_address, sizeof(sock_address));

 if(result == -1)
 {
   printf("Connection not successful\n");
   exit(1);
 }


char *line = NULL;
int lineSize = 0;
printf("type and we will send(type exit)...");
while(1)
{
  int char_count = getline(&line, &lineSize, stdin); 

  if(char_count > 0)
  {
      if(strcmp(line, "exit") == 0)
      {
        break;
      }
  }

  int amount_sent = send(socketFD, line, char_count,0);

}

close(socketFD);
  
}