//
//  TcpServer.cpp
//  server
//
//  Created by Michael Knight on 10/30/15.
//  Copyright © 2015 Michael Knight. All rights reserved.
//

#include "TcpServer.hpp"
#include <vector>
using namespace std;

int TcpServer::Start(int port)
{
    struct sockaddr_in serverAdddress;
    
    _socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    
    if (_socketFileDescriptor < 0)
        return -1;
    
    bzero((char *) &serverAdddress,
          sizeof(serverAdddress));
    
    serverAdddress.sin_family = AF_INET;
    serverAdddress.sin_addr.s_addr = INADDR_ANY;
    serverAdddress.sin_port = htons(port);
    
    if (bind(_socketFileDescriptor,
            (struct sockaddr *) &serverAdddress,
                sizeof(serverAdddress)) <0)
         return -1;
    
    listen(_socketFileDescriptor,5);

    return 1;
}
TcpServer::~TcpServer()
{
    close(_socketFileDescriptor);
    close(_currentFileDescriptor);
}
int TcpServer::Listen()
{
    socklen_t clientLen;
    sockaddr_in address;
    clientLen = sizeof(address);
    _currentFileDescriptor = accept(_socketFileDescriptor,
                                    (struct sockaddr *) &address,
                                    &clientLen);
    return _currentFileDescriptor;
}
PackedMessage_t* TcpServer::ReadMessage()
{
    char buffer[1024];
    PackedMessage_t* message = new PackedMessage_t();
    
    while (!message->Complete())
    {
       long bytes = read(_currentFileDescriptor,buffer,1024);
       if (bytes == -1)
           return NULL;
        
       message->AppendData(buffer, bytes);
    }
    return message;
}
long TcpServer::Write(PackedMessage_t message)
{
    return write(_currentFileDescriptor,message.buffer,message.messageSize);
}
