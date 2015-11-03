//
//  main.cpp
//  client
//
//  Created by Michael Knight on 11/1/15.
//  Copyright (c) 2015 Michael Knight. All rights reserved.
//

#include <iostream>
#include "TcpClient.hpp"
#include <errno.h>

int main(int argc, const char * argv[]) {
    TcpClient client;
    const char * host;
    int port;
    const char * command;
    int value;
    const char * data = NULL;
    
    if (argc < 5)
    {
        printf("Usage: client host [port] [command] [leaseid|size] [duration|data]\n");
        printf("\t host: host running the server. example: localhost\n");
        printf("\t port: port the server is connect to. example: 412.\n");
        printf("\t command: command to run.\n");
        printf("\t\t lease: lease some some memmory (Note: set requires duration argument in MS)\n");
        printf("\t\t get: get memory for the provided lease id\n");
        printf("\t\t set: set memory for the provided lease id. (Note: set requires data argument)\n");
        printf("\n\n");
        printf("#example run:\n");
        printf("#lease some memory from the server\n");
        printf("client localhost 412 lease 1024 5000\n");
        printf(">You have leased 1024 bytes for 5000 MS with id 123\n");
        printf("client localhost set 123  \"this is some text\"\n");
        printf(">Data set for lease 123\n");
        printf("client localhost get 123\n");
        printf(">Data get for lease 123: this is some text\n");
        exit(0);
    }
    
    //pull info from command line
    host = argv[1];
    port = atoi(argv[2]);
    command = argv[3];
    value = atoi(argv[4]);
    if (argc > 5)
        data = argv[5];
    
    bool ok = strcmp("lease", command) == 0 || strcmp("get", command) == 0 || strcmp("set", command) == 0;
    if (!ok)
    {
        printf("unknown command:%s\n",command);
        exit(0);
    }
    
    int err = client.Start(host, port);
    if (err <0)
    {
        if (err == -2)
            printf("Unable to resolve host\n");
        else
            printf("Error connecting: %s (%d)\n",strerror(errno),errno);
        exit(err);
    }
    if (strcmp("lease", command) == 0)
    {
        int duration = atoi(data);
        PackedMessage_t lease = MemProtocol::CreateLeaseMessage(value, duration);
        client.Write(lease);
        PackedMessage_t leasedMessage = client.ReadMessage();
        LeasedMessage_t lm =  MemProtocol::ReadLeasedMessage(leasedMessage);
        if (lm.leaseId >0)
        {
            printf("You have leased %d bytes for %d MS with id %d\n",
                   value,
                   duration,
                   lm.leaseId);
        }
        else
        {
            printf("Failed to create lease due to insufficient resources on the server.\n");
        }
        
    }
    if (strcmp("get", command) == 0)
    {
        PackedMessage_t access = MemProtocol::CreateAccessDataMessage(value);
        client.Write(access);
        PackedMessage_t pm  = client.ReadMessage();
        DataMessage_t dm = MemProtocol::ReadDataMessage(pm);
        if (dm.buffer)
            printf("Data get for lease %d: %s\n",value,dm.buffer);
        else
            printf("Data not found for %d.\n",value);
        
    }
    if (strcmp("set", command) == 0)
    {
        PackedMessage_t access = MemProtocol::CreateDataMessage(data, strlen(data)+1, value, SetData);
        client.Write(access);
        PackedMessage_t pm  = client.ReadMessage();
        DataSetMessage_t dm = MemProtocol::ReadDataSetMessage(pm);
        if (dm.error >= 0)
            printf("Data set for lease %d\n",value);
        else
            printf("Error: unable set for lease %d (%d)\n",value,dm.error);
    }
    
    return 0;
}
