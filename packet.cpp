#include <string>
#include <cstring>
#include <ctime>

enum class MessageType {
    CONNECT,
    DISCONNECT,
    PUBLIC_MESSAGE,
    PRIVATE_MESSAGE,
    SYSTEM_MESSAGE
};

struct ChatMessage {
    MessageType type;
    char sender[32];
    char recipient[32];  // For private messages
    char content[1024];
    time_t timestamp;
};

struct Packet {
    ChatMessage message;
    
    static Packet createConnect(const std::string& username) {
        Packet packet;
        packet.message.type = MessageType::CONNECT;
        strncpy(packet.message.sender, username.c_str(), sizeof(packet.message.sender));
        packet.message.timestamp = std::time(nullptr);
        return packet;
    }
    
    static Packet createPublicMessage(const std::string& sender, const std::string& content) {
        Packet packet;
        packet.message.type = MessageType::PUBLIC_MESSAGE;
        strncpy(packet.message.sender, sender.c_str(), sizeof(packet.message.sender));
        strncpy(packet.message.content, content.c_str(), sizeof(packet.message.content));
        packet.message.timestamp = std::time(nullptr);
        return packet;
    }
    
    static Packet createPrivateMessage(const std::string& sender, 
                                     const std::string& recipient, 
                                     const std::string& content) {
        Packet packet;
        packet.message.type = MessageType::PRIVATE_MESSAGE;
        strncpy(packet.message.sender, sender.c_str(), sizeof(packet.message.sender));
        strncpy(packet.message.recipient, recipient.c_str(), sizeof(packet.message.recipient));
        strncpy(packet.message.content, content.c_str(), sizeof(packet.message.content));
        packet.message.timestamp = std::time(nullptr);
        return packet;
    }
};