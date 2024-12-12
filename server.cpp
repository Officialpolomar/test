#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <map>
#include <deque>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include "packet.cpp"
#include "thread_pool.cpp"

class ChatServer {
    struct ClientInfo {
        int socket;
        std::string username;
        bool connected;
    };

    std::map<std::string, ClientInfo> clients;
    std::deque<ChatMessage> message_history;
    static const size_t MAX_HISTORY = 100;
    
    std::mutex clients_mutex;
    std::mutex history_mutex;
    ThreadPool thread_pool;

public:
    ChatServer() : thread_pool(4) {}

    void start() {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            std::cerr << "Failed to create socket" << std::endl;
            return;
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Failed to bind socket" << std::endl;
            close(server_fd);
            return;
        }

        listen(server_fd, 10);
        std::cout << "Server started on port 8080" << std::endl;

        while (true) {
            int client_socket = accept(server_fd, nullptr, nullptr);
            if (client_socket < 0) {
                std::cerr << "Failed to accept connection" << std::endl;
                continue;
            }

            thread_pool.enqueue([this, client_socket]() {
                handleNewConnection(client_socket);
            });
        }
    }

    void handleNewConnection(int client_socket) {
        Packet packet;
        if (recv(client_socket, &packet, sizeof(packet), 0) > 0) {
            if (packet.message.type == MessageType::CONNECT) {
                std::string username(packet.message.sender);
                
                {
                    std::lock_guard<std::mutex> lock(clients_mutex);
                    clients[username] = {client_socket, username, true};
                }

                // Send message history
                {
                    std::lock_guard<std::mutex> history_lock(history_mutex);
                    for (const auto& msg : message_history) {
                        Packet history_packet;
                        history_packet.message = msg;
                        send(client_socket, &history_packet, sizeof(Packet), 0);
                    }
                }

                // Broadcast connection message
                broadcastSystemMessage(username + " has joined the chat");
                handleClientMessages(client_socket, username);
            }
        }
    }

private:
    void handleClientMessages(int client_socket, const std::string& username) {
        Packet packet;
        while (recv(client_socket, &packet, sizeof(packet), 0) > 0) {
            switch (packet.message.type) {
                case MessageType::PUBLIC_MESSAGE:
                    handlePublicMessage(packet.message);
                    break;
                    
                case MessageType::PRIVATE_MESSAGE:
                    handlePrivateMessage(packet.message);
                    break;
                    
                case MessageType::DISCONNECT:
                    handleDisconnect(username);
                    return;

                case MessageType::CONNECT:
                    // Already handled
                    break;

                case MessageType::SYSTEM_MESSAGE:
                    // System messages are only sent by server
                    break;
            }
        }
        
        handleDisconnect(username);
    }

    void handlePublicMessage(const ChatMessage& message) {
        std::time_t t = message.timestamp;
        std::cout << "[" << std::put_time(std::localtime(&t), "%H:%M:%S") << "] "
                 << message.sender << ": " << message.content << std::endl;

        {
            std::lock_guard<std::mutex> history_lock(history_mutex);
            message_history.push_back(message);
            if (message_history.size() > MAX_HISTORY) {
                message_history.pop_front();
            }
        }

        broadcastMessage(message);
    }

    void handlePrivateMessage(const ChatMessage& message) {
        std::string recipient(message.recipient);
        
        std::lock_guard<std::mutex> lock(clients_mutex);
        auto it = clients.find(recipient);
        if (it != clients.end() && it->second.connected) {
            Packet packet;
            packet.message = message;
            send(it->second.socket, &packet, sizeof(Packet), 0);
        }
    }

    void handleDisconnect(const std::string& username) {
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            auto it = clients.find(username);
            if (it != clients.end()) {
                close(it->second.socket);
                clients.erase(it);
            }
        }
        
        broadcastSystemMessage(username + " has left the chat");
    }

    void broadcastMessage(const ChatMessage& message) {
        Packet packet;
        packet.message = message;
        
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (const auto& client : clients) {
            if (client.second.connected) {
                send(client.second.socket, &packet, sizeof(Packet), 0);
            }
        }
    }

    void broadcastSystemMessage(const std::string& message) {
        Packet packet;
        packet.message.type = MessageType::SYSTEM_MESSAGE;
        strncpy(packet.message.content, message.c_str(), sizeof(packet.message.content));
        packet.message.timestamp = std::time(nullptr);
        
        broadcastMessage(packet.message);
    }
};

int main() {
    ChatServer server;
    server.start();
    return 0;
}