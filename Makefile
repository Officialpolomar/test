# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

# Directories
BUILD_DIR = out
CLIENT_SOURCES = client.cpp packet.cpp
SERVER_SOURCES = server.cpp packet.cpp
HEADERS = util.h

# Object files
CLIENT_OBJECTS = $(addprefix $(BUILD_DIR)/, $(CLIENT_SOURCES:.cpp=.o))
SERVER_OBJECTS = $(addprefix $(BUILD_DIR)/, $(SERVER_SOURCES:.cpp=.o))

# Output binaries
CLIENT_TARGET = $(BUILD_DIR)/client
SERVER_TARGET = $(BUILD_DIR)/server

# Default target: build both executables
all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Ensure the build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Client executable
$(CLIENT_TARGET): $(CLIENT_OBJECTS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(CLIENT_OBJECTS)

# Server executable
$(SERVER_TARGET): $(SERVER_OBJECTS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_OBJECTS)

# Compile each source file into an object file
$(BUILD_DIR)/%.o: %.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(BUILD_DIR)

# Run commands
run-client: $(CLIENT_TARGET)
	@echo "Running client..."
	./$(CLIENT_TARGET)

run-server: $(SERVER_TARGET)
	@echo "Running server..."
	./$(SERVER_TARGET)

# Phony targets
.PHONY: all clean run-client run-server
