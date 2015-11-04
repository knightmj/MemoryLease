//
//  TcpClient.cpp
//  client
//
//  Created by Michael Knight on 11/1/15.
//  Copyright (c) 2015 Michael Knight. All rights reserved.
//

#include "TcpClient.hpp"
#include <errno.h>

PackedMessage_t TcpClient::ReadMessage()
{
    char buffer[1024];
    PackedMessage_t message;
    
    while (!message.Complete())
    {
        long bytes = read(_currentFileDescriptor,buffer,1024);
        message.AppendData(buffer, bytes);
    }
    return message;
}
long TcpClient::Write(PackedMessage_t message)
{
    return write(_currentFileDescriptor,message.buffer,message.messageSize);
}
int TcpClient::Start(const char* host, int port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
    _currentFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (_currentFileDescriptor < 0)
        return -1;
    
    server = gethostbyname(host);
    if (server == NULL)
        return -2;
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);
    int err =connect(_currentFileDescriptor,(struct sockaddr *) &serv_addr,sizeof(serv_addr));

    if ( err < 0)
    {
        return err;
    }
    return 0;
}