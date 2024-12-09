# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

# Directories
SRC_DIR = src
BUILD_DIR = build
CLIENT_DIR = $(SRC_DIR)/client
SERVER_DIR = $(SRC_DIR)/server
PROTOCOL_DIR = $(SRC_DIR)/protocol

# Source files
CLIENT_SOURCES = $(CLIENT_DIR)/client.cpp $(PROTOCOL_DIR)/packet.cpp
SERVER_SOURCES = $(SERVER_DIR)/server.cpp $(PROTOCOL_DIR)/packet.cpp

# Object files
CLIENT_OBJECTS = $(BUILD_DIR)/client.o $(BUILD_DIR)/packet_client.o
SERVER_OBJECTS = $(BUILD_DIR)/server.o $(BUILD_DIR)/packet_server.o

# Output binaries
CLIENT_TARGET = $(BUILD_DIR)/client
SERVER_TARGET = $(BUILD_DIR)/server

# Default target: build both executables
all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Client executable
$(CLIENT_TARGET): $(CLIENT_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(CLIENT_OBJECTS)

# Server executable
$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_OBJECTS)

# Compile client object files
$(BUILD_DIR)/client.o: $(CLIENT_DIR)/client.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/packet_client.o: $(PROTOCOL_DIR)/packet.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile server object files
$(BUILD_DIR)/server.o: $(SERVER_DIR)/server.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/packet_server.o: $(PROTOCOL_DIR)/packet.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object and executable files
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/client $(BUILD_DIR)/server

# Run commands
run-client: $(CLIENT_TARGET)
	@echo "Running client..."
	./$(CLIENT_TARGET)

run-server: $(SERVER_TARGET)
	@echo "Running server..."
	./$(SERVER_TARGET)

# Phony targets
.PHONY: all clean run-client run-server
