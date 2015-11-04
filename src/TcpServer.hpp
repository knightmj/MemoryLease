//
//  TcpServer.hpp
//  server
//
//  Created by Michael Knight on 10/30/15.
//  Copyright © 2015 Michael Knight. All rights reserved.
//

#ifndef TcpServer_hpp
#define TcpServer_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "MemProtocol.hpp"

class TcpServer
{
public:
    // start a server 
    // port: port to bind to
    int Start(int port);
    // Listen for an incoming connection
    // returns: id for the socket connectin
    int Listen();

    // read a packed message from the server
    // return: A valid packed message or NULL if the client fails 
    // to send a a valid message.
    PackedMessage_t* ReadMessage();
    
    // write a message to the client
    // returns: number of bytes written
    long Write(PackedMessage_t message);
    
    ~TcpServer();
private:
    int _socketFileDescriptor;
    int _currentFileDescriptor;
};
#endif /* TcpServer_hpp */
