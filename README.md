# Final Project Group Chat Application
Aren Egwuekwe and Omar White-Evans

## Dependencies

### For MacOS
```bash
# Install FLTK using Homebrew
brew install fltk
```

### For Linux
```bash
# Install FLTK and development tools
sudo apt-get update
sudo apt-get install libfltk1.3-dev build-essential
```

## Building
```bash
# Clean any previous builds
make clean

# Build both server and client
make
```

## Running

You'll need at least two terminal windows, one for the server and one for each client.

### Start the Server
```bash
make run-server
```

### Start Clients
Open new terminals for each client:
```bash
make run-client
# Enter your username when prompted
```

## Files:

    server.cpp --> Server logic, handling connections, message routing, and overall server management.

    thread_pool.cpp --> Manages a pool of threads to handle concurrent client connections efficiently.

    packet.cpp --> Defines the structure and handling of data packets exchanged between the server and clients.

    client.cpp --> Client logic, handling user input, message sending, and receiving messages from the server.

    packet.cpp --> (Same as server-side) Defines the structure and handling of data packets.

    Makefile: Defines the build process for the application, specifying compilation and linking steps.


Aren Egwuekwe:  
    - Implemented audio and group chat support  
    - Implemented client and server logic  
    - Completed final report and protocol design  

Omar White-Evans:  
    - Implemented FLTK GUI for project  
    - Completed final report on documentation   
    - Authored README  
  
