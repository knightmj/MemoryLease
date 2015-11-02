//
//  TcpClient.h
//  client
//
//  Created by Michael Knight on 11/1/15.
//  Copyright (c) 2015 Michael Knight. All rights reserved.
//

#ifndef __client__TcpClient__
#define __client__TcpClient__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "MemProtocol.hpp"

class TcpClient
{
public:
    int Start(const char * host, int port);
    PackedMessage_t ReadMessage();
    long Write(PackedMessage_t message);
private:
    int _currentFileDescriptor;
};

#endif /* defined(__client__TcpClient__) */
