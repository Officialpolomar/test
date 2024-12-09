#ifndef PACKET_HPP
#define PACKET_HPP

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

Packet createPacket(const std::string& header, const std::string& message);

void sendPacket(int socket_fd, const Packet& packet);

Packet receivePacket(int socket_fd);

#endif // PACKET_HPP