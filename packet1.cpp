#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <thread>
#include <sys/socket.h>

struct Packet {
    char header[10];   // e.g., "TEXT", "AUDIO", "VIDEO"
    char data[1024];   // Payload
};

Packet createPacket(const std::string& header, const std::string& message) {
    Packet packet;
    
    strncpy(packet.header, header.c_str(), sizeof(packet.header));
    strncpy(packet.data, message.c_str(), sizeof(packet.data));
    
    return packet;
}

void sendPacket(int socket_fd, const Packet& packet) {
    send(socket_fd, &packet, sizeof(packet), 0);
}

Packet receivePacket(int socket_fd) {
    Packet packet;
    recv(socket_fd, &packet, sizeof(packet), 0);
    
    return packet;
}
