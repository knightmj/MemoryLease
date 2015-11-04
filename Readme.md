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
Similar in nature to memchache, MemoryLease enables leased remote access to the memory on a computer.  It provides methods to get and set that memory by a lease id. It implements a single threaded TcpServer or TcpClient, and a Buddy Memory Allocator. 

## TcpServer and TcpClient

The server and the client communicate via a binary protocol that contains 5 different types of messages.

*  Lease: Request a lease of a size for a duration
*  Leased: A lease has been requested and was provided or not
*  DataSetMessage: set the contents of a lease in memory
*  AccessDataMessage: get the contents of the leased memory
*  DataSetMessage: confirm or provide error information on if data was set

These messages are covered into binary form and sent across the TCP socket.

## Buddy Memory Allocator

The Buddy Memory Allocator is used to decrease fragmentation over time by providing a simple mechanism for coalescing freed memory. The Buddy Memory allocator reduces the speed and overhead required to re-combine contiguous blocks of memory by always ensuring that each child is half the memory of it's parent. This implementation uses a triply linked tree structure. Because of this if at any point both children are free the parent can be combined without having to compare entries in a free list, or attempt more complicated methods of defragmentation. 

However the allocator can still be sub-optimal when it comes to internal fragmentation and other strategies may be more beneficial in areas where you wish to trade speed for fast defragmentation.

### Allocating memory

When memory is requested, the allocator travels the tree to find the best fitting available node. It will then split that node in in half until it reaches a minimum size or the same size as the request rounded up to the nearest power of two. As it does so it creates smaller free blocks of available memory that can be used by others.  

When it has created a node that is similar in size, it marks it as allocated and returns it for use.

### Freeing memory
When memory is released back to the system, we traverse the link to that node's parent. If both of it's children's memory is available those children are removed and the combined block can now be accessed. This process is continued recursively until we find a parent that still has an allocated child.

### More info about Buddy Memory Allocation
https://en.wikipedia.org/wiki/Buddy_memory_allocation 

# Other Considerations

Because of the nature of this assignment 3rd party libraries where not used. However boost would likely have improved code functionality and performance with better TCP libraries and memory allocation algorithms.  

# Future Work
MemoryLease only works for a single client, and a client can only run 1 command at a time. Allowing clients to remain connected and servicing multiple concurrent clients would improve use but was not done at this time.

MemoryLease speaks a binary protocol but that does not account for endianness.  If the client and server were to be compiled on computers with different byte orders they would not be able to communicate without changes to the binary protocol. 

MemoryLease uses a integer to identify a lease holder. It likely would improve usage if a UUID was used. 