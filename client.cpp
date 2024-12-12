#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <thread>
#include <sstream>
#include <iomanip>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <ctime>
#include "packet.cpp"

class ChatClient : public Fl_Window {
private:
    Fl_Text_Display* display;
    Fl_Text_Buffer* buffer;
    Fl_Input* input;
    Fl_Button* send_button;
    
    int socket_fd;
    std::string username;
    std::thread receive_thread;
    std::mutex buffer_mutex;
    bool running;

public:
    ChatClient(int w, int h, const char* title, int sock, const std::string& user)
        : Fl_Window(w, h, title), socket_fd(sock), username(user), running(true) {
        
        std::cout << "Initializing ChatClient..." << std::endl;
        
        // Initialize FLTK window
        this->begin();
        
        // Create buffer before display
        buffer = new Fl_Text_Buffer();
        if (!buffer) {
            std::cerr << "Failed to create text buffer" << std::endl;
            return;
        }
        
        display = new Fl_Text_Display(10, 10, w-20, h-80);
        if (!display) {
            std::cerr << "Failed to create text display" << std::endl;
            return;
        }
        display->buffer(buffer);
        display->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
        
        input = new Fl_Input(10, h-60, w-160, 30, "");  
        if (!input) {
            std::cerr << "Failed to create input field" << std::endl;
            return;
        }
        
        
        send_button = new Fl_Button(w-140, h-60, 60, 30, "Send");  
        if (!send_button) {
            std::cerr << "Failed to create send button" << std::endl;
            return;
        }
        send_button->callback(static_send_cb, this);

        
        Fl_Button* quit_button = new Fl_Button(w-70, h-60, 60, 30, "Quit");  
        if (!quit_button) {
            std::cerr << "Failed to create quit button" << std::endl;
            return;
        }
        quit_button->callback(static_quit_cb, this);

        
        resizable(display);  
        set_modal();  

        
        input->when(FL_WHEN_ENTER_KEY);
        input->callback(static_input_cb, this);
        
        this->end();
        
        // Start receive thread
        std::cout << "Starting receive thread..." << std::endl;
        try {
            receive_thread = std::thread(&ChatClient::receiveMessages, this);
            receive_thread.detach();
        } catch (const std::exception& e) {
            std::cerr << "Failed to create receive thread: " << e.what() << std::endl;
            return;
        }
        
        std::cout << "ChatClient initialization complete" << std::endl;
        appendMessage("Connected to chat server");
    }

    ~ChatClient() {
        std::cout << "ChatClient destructor called" << std::endl;
        running = false;
        if (buffer) delete buffer;
        close(socket_fd);
    }

private:
    static void static_send_cb(Fl_Widget*, void* v) {
        if (v) {
            ((ChatClient*)v)->send_cb();
        }
    }

    static void static_input_cb(Fl_Widget* w, void* v) {
        if (v) {
            ((ChatClient*)v)->send_cb();
        }
    }

    static void static_quit_cb(Fl_Widget*, void* v) {
        if (v) {
            ChatClient* client = (ChatClient*)v;
            Packet packet;
            packet.message.type = MessageType::DISCONNECT;
            strncpy(packet.message.sender, client->username.c_str(), sizeof(packet.message.sender));
            send(client->socket_fd, &packet, sizeof(packet), 0);
            
            client->hide();
        }
    }

    void send_cb() {
        if (!input) return;
        
        std::string message = input->value();
        input->value("");
        
        if (message.empty()) return;
        
        try {
            if (message[0] == '/') {
                handleCommand(message);
            } else {
                Packet packet = Packet::createPublicMessage(username, message);
                if (send(socket_fd, &packet, sizeof(packet), 0) < 0) {
                    appendMessage("Error: Failed to send message");
                }
            }
        } catch (const std::exception& e) {
            appendMessage("Error sending message: " + std::string(e.what()));
        }
    }

    void handleCommand(const std::string& cmd) {
        std::istringstream iss(cmd);
        std::string command, recipient, message;
        iss >> command;
        
        if (command == "/msg") {
            iss >> recipient;
            std::getline(iss, message);
            if (!message.empty() && !recipient.empty()) {
                message = message.substr(1);
                try {
                    Packet packet = Packet::createPrivateMessage(username, recipient, message);
                    if (send(socket_fd, &packet, sizeof(packet), 0) < 0) {
                        appendMessage("Error: Failed to send private message");
                        return;
                    }
                    appendMessage("(PM to " + recipient + ") " + message);
                } catch (const std::exception& e) {
                    appendMessage("Error sending private message: " + std::string(e.what()));
                }
            }
        }
    }

    void receiveMessages() {
        std::cout << "Receive thread started" << std::endl;
        Packet packet;
        
        while (running && recv(socket_fd, &packet, sizeof(packet), 0) > 0) {
            try {
                std::string formatted_message;
                
                switch (packet.message.type) {
                    case MessageType::PUBLIC_MESSAGE:
                        formatted_message = formatPublicMessage(packet.message);
                        break;
                        
                    case MessageType::PRIVATE_MESSAGE:
                        formatted_message = formatPrivateMessage(packet.message);
                        break;
                        
                    case MessageType::SYSTEM_MESSAGE:
                        formatted_message = formatSystemMessage(packet.message);
                        break;

                    case MessageType::CONNECT:
                    case MessageType::DISCONNECT:
                        break;
                }
                
                if (!formatted_message.empty()) {
                    appendMessage(formatted_message);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing received message: " << e.what() << std::endl;
            }
        }
        
        if (running) {
            appendMessage("Disconnected from server");
        }
        std::cout << "Receive thread ending" << std::endl;
    }

    void appendMessage(const std::string& message) {
        if (!buffer || !display) return;
        
        try {
            std::lock_guard<std::mutex> lock(buffer_mutex);
            buffer->append((message + "\n").c_str());
            display->scroll(buffer->count_lines(0, buffer->length()), 0);
            Fl::awake();
        } catch (const std::exception& e) {
            std::cerr << "Error appending message: " << e.what() << std::endl;
        }
    }

    std::string formatPublicMessage(const ChatMessage& msg) {
        std::time_t t = msg.timestamp;
        std::stringstream ss;
        ss << "[" << std::put_time(std::localtime(&t), "%H:%M:%S") << "] "
           << msg.sender << ": " << msg.content;
        return ss.str();
    }

    std::string formatPrivateMessage(const ChatMessage& msg) {
        std::time_t t = msg.timestamp;
        std::stringstream ss;
        ss << "[" << std::put_time(std::localtime(&t), "%H:%M:%S") << "] "
           << "(PM from " << msg.sender << ") " << msg.content;
        return ss.str();
    }

    std::string formatSystemMessage(const ChatMessage& msg) {
        std::time_t t = msg.timestamp;
        std::stringstream ss;
        ss << "[" << std::put_time(std::localtime(&t), "%H:%M:%S") << "] "
           << "*** " << msg.content << " ***";
        return ss.str();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <username>" << std::endl;
        return 1;
    }

    std::string username = argv[1];
    std::cout << "Starting client with username: " << username << std::endl;

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    std::cout << "Connecting to server..." << std::endl;
    
    if (connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect to server" << std::endl;
        close(client_fd);
        return 1;
    }

    std::cout << "Connected to server, sending connect packet..." << std::endl;
    
    Packet connect_packet = Packet::createConnect(username);
    if (send(client_fd, &connect_packet, sizeof(connect_packet), 0) < 0) {
        std::cerr << "Failed to send connect packet" << std::endl;
        close(client_fd);
        return 1;
    }

    std::cout << "Creating window..." << std::endl;
    
    ChatClient* window = new ChatClient(800, 600, "Chat Client", client_fd, username);
    window->show();
    
    std::cout << "Starting FLTK main loop..." << std::endl;
    return Fl::run();
}