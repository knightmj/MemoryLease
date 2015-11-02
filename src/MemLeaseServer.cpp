//
//  main.cpp
//  server
//
//  Created by Michael Knight on 10/30/15.
//  Copyright © 2015 Michael Knight. All rights reserved.
//

#include <iostream>
#include <cstring>
#include "MemProtocol.hpp"
#include "TcpServer.hpp"
#include "LeasedMemoryContainer.hpp"
#include <errno.h>

int main(int argc, const char * argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s [port number] [max memory]\n",argv[0]);
        exit(0);
    }
    
    int port = atoi(argv[1]);
    int memSize = atoi(argv[2]);
    
    TcpServer server;
    printf("starting server on port %d\n",port);
    
    int err = server.Start(port);
    if (err<0)
    {
        printf("Error: Unable to start server. %s (%d)\n",strerror(errno),errno);
        exit(err);
    }
    LeasedMemoryContainer container;
    
    container.CreateMemory(memSize);
    while (true)
    {
         printf("listening for client\n");
        int err = server.Listen();
        if (err<0)
            exit(err);
       
        PackedMessage_t* msg = server.ReadMessage();
        if (!msg)
            continue;
        //make sure the leases are up to date
        container.CleanLeases();
        
        switch (msg->Type()) {
            case Lease:
            {
                LeaseMessage_t leaseMessage = MemProtocol::ReadLeaseMessage(*msg);
                int lease = container.RequestLease(leaseMessage.leaseSize, leaseMessage.leasedDuration);
                PackedMessage_t leasedMessage = MemProtocol::CreateLeasedMessage(lease);
                server.Write(leasedMessage);
            }
                break;
            case AccessData:
            {
                //the client would like to get data for this lease
                AccessDataMessage_t accessMessage = MemProtocol::ReadAccessMessage(*msg);
                char * mem;
                long size;
                container.GetMemory(&mem, size, accessMessage.leaseId);
                PackedMessage_t dm = MemProtocol::CreateDataMessage(mem, size, accessMessage.leaseId, AccessData);
                server.Write(dm);
            }
                break;
            case SetData:
            {
                DataMessage_t dm = MemProtocol::ReadDataMessage(*msg);
                int error = container.SetMemory(dm.buffer, dm.bufferSize, dm.leaseId);
                PackedMessage_t pm = MemProtocol::CreateDataSetMessage(error);
                server.Write(pm);
            }
            default:
                break;
        }
    }
    return 0;
}
