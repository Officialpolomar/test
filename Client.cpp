#include <iostream>
#include <thread>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>

void receiveMessages(int socket_fd)
{
  char buffer[1024];
  while (recv(socket_fd, buffer, sizeof(buffer), 0) > 0)
  {
    std::cout << "Message: " << buffer << std::endl;
  }
}

int main()
{
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  connect(client_fd, (sockaddr *)&server_addr, sizeof(server_addr));
  
  std::cout << "Connected to server" << std::endl;
  std::thread(receiveMessages, client_fd).detach();
  std::string message;

  while (true)
  {
    std::getline(std::cin, message);
    send(client_fd, message.c_str(), message.size(), 0);
  }

  return 0;
}