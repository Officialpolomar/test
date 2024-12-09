#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>

std::vector<int> client_sockets;
std::mutex client_mutex;

void handleClient(int client_socket) {
    char buffer[1024];

    while (recv(client_socket, buffer, sizeof(buffer), 0) > 0) {
        std::cout << "Message received: " << buffer << std::endl;

        // Broadcast message to all clients
        std::lock_guard<std::mutex> lock(client_mutex);
        for (int socket : client_sockets) {
            if (socket != client_socket) {
                send(socket, buffer, sizeof(buffer), 0);
            }
        }
    }
    close(socket);
}

int main() {
    // Create a socket for the server
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Define the server's address information
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // Listen on port 8080
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    // Start listening for incoming connections
    listen(server_fd, 10);

    std::cout << "Server started on port 8080" << std::endl;

    while (true) {
        // Accept a new client connection
        int client_socket = accept(server_fd, nullptr, nullptr);

        // Add the client socket to the vector of client sockets
        std::lock_guard<std::mutex> lock(client_mutex);
        client_sockets.push_back(client_socket);

        // Create a new thread to handle the client
        std::thread(handleClient, client_socket).detach();
    }
}