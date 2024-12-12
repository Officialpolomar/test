# test

To build and run this application, input the commands:
make, 
    make run-server,
    make run-client

Files:

    server.cpp --> Server logic, handling connections, message routing, and overall server management.

    thread_pool.cpp --> Manages a pool of threads to handle concurrent client connections efficiently.

    packet.cpp --> Defines the structure and handling of data packets exchanged between the server and clients.

    producer_consumer.cpp --> Implements a producer-consumer pattern for managing message queues and ensuring smooth data flow.

    client.cpp --> Client logic, handling user input, message sending, and receiving messages from the server.

    audio.cpp --> Handles audio-related functionalities like text or audio message support.

    packet.cpp --> (Same as server-side) Defines the structure and handling of data packets.

    Makefile: Defines the build process for the application, specifying compilation and linking steps.


Aren Egwuekwr:
    - Implemented audio and group chat support
    - Completed protocol design on documentation

Omar White-Evans:
    - Implemented FLTK GUI for project
    - Completed final report on documentation 
    - Author of this README