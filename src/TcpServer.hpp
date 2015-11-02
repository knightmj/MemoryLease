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
    int Start(int port);
    int Listen();
    PackedMessage_t* ReadMessage();
    long Write(PackedMessage_t message);
    ~TcpServer();
private:
    int _socketFileDescriptor;
    int _currentFileDescriptor;
};
#endif /* TcpServer_hpp */
