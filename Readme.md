# MemoryLease
MemoryLease is a client and server application written in C++. Please read More Details for more information on how it works. 

# Build MemoryLease
Memory Lease can be built with `make` and the provide Makeflile assuming g++ is installed. Once make is run a `bin` directory will be created with the server and client binaries like so.

    cd [base_dir]
    make
    >mkdir -p bin
    >g++ -o bin/server src/MemLeaseServer.cpp src/MemProtocol.cpp src/TcpServer.cpp src/LeasedMemoryContainer.cpp
    >g++ -o bin/client src/MemLeaseClient.cpp src/MemProtocol.cpp src/TcpClient.cpp
# Running MemoryLease
MemoryLese is a client and a server so you must first start the server and then connect to it with the client.

### Running the Server
First let's run the server. The server requires enough permissions to bind to an open port. So we will run the server with the sudo command.  

The takes two arguments. The first is the port we want to use. The second the maximum amount of memory we want to lease.

```
cd bin
sudo ./server 412 2048
starting server on port 412
listening for client
```

You may want to either run the server in the background or open a new shell to run the client. Given that the server will log connection info to standard out it's likely a good idea to start a new shell.

### Running The Client
The client accepts a series of command line arguments

    Usage: client host [port] [command] [leaseid|size] [duration|data]
        host: host running the server. example: localhost
         port: port the server is connect to. example: 412.
         command: command to run.
            lease: lease some some memory (Note: set requires duration argument in MS)
            get: get memory for the provided lease id
            set: set memory for the provided lease id. (Note: set requires data argument)

##### example run:
lease some memory from the server

    client localhost 412 lease 1024 5000
    >You have leased 1024 bytes for 5000 MS with id 123
    client localhost set 123  "this is some text"
    >Data set for lease 123
    client localhost get 123
    >Data get for lease 123: this is some text

# More Details
Similar in nature to memchache, MemoryLease enables leased remote access to the memory on a computer.  It provides methods to get and set that memory by a lease id. It implements a single threaded TcpServer, TcpClient, and Buddy Memory Allocator. https://en.wikipedia.org/wiki/Buddy_memory_allocation The Buddy Memory Allocator is used to decrease fragmentation over time by providing a simple mechanism for coalescing freed memory. 

Because of the nature of this assignment 3rd party libraries where not used. However boost would likely have improved code functionality and performance with better TCP libraries and memory allocation algorithms.  

# Future Work
MemoryLease only works for a single client, and a client can only run 1 command at a time. Allowing clients to remain connected and servicing multiple concurrent clients would improve use but was not done at this time.

MemoryLease speaks a binary protocol but that does not account for endianness.  If the client and server were to be compiled on computers with different byte orders they would not be able to communicate without changes to the binary protocol. 

MemoryLease uses a integer to identify a lease holder. It likely would improve usage if a UUID was used. 