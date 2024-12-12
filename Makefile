# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread
FLTK_FLAGS = $(shell fltk-config --use-images --cxxflags --ldflags)
INCLUDES = -I/opt/homebrew/include
LIBS = -L/opt/homebrew/lib

# Directories
BUILD_DIR = build
SRC_DIR = .

# Output executables
SERVER_TARGET = $(BUILD_DIR)/server
CLIENT_TARGET = $(BUILD_DIR)/client

# Source files
SERVER_SOURCES = server.cpp
CLIENT_SOURCES = client.cpp

# Default username for testing client
DEFAULT_USERNAME = TestUser

# Create build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

.PHONY: all clean run-server run-client

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_SOURCES)
	@echo "Building server..."
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Server build complete"

$(CLIENT_TARGET): $(CLIENT_SOURCES)
	@echo "Building client..."
	$(CXX) $(CXXFLAGS) $(FLTK_FLAGS) $(INCLUDES) $(LIBS) $^ -o $@
	@echo "Client build complete"

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)
	@echo "Clean complete"

run-server: $(SERVER_TARGET)
	@echo "Starting server..."
	./$(SERVER_TARGET)

run-client: $(CLIENT_TARGET)
	@echo "Starting client..."
	@read -p "Enter username (default: $(DEFAULT_USERNAME)): " username; \
	if [ -z "$$username" ]; then \
		./$(CLIENT_TARGET) $(DEFAULT_USERNAME); \
	else \
		./$(CLIENT_TARGET) "$$username"; \
	fi