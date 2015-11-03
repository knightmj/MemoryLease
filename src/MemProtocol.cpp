//
//  MemProtocol.cpp
//  server
//
//  Created by Michael Knight on 10/31/15.
//  Copyright © 2015 Michael Knight. All rights reserved.
//

#include "MemProtocol.hpp"
#include <cstring>


PackedMessage_t CreateMessage(MessageType type, char * payload, long payloadSize)
{
    PackedMessage_t pm;
    pm.messageSize = MESSAGE_OVERHEAD + payloadSize;
    int offset =0 ;
    
    //message type
    pm.buffer = new char[pm.messageSize];
    memcpy(pm.buffer,&type,sizeof(int));
    offset+= sizeof(int);
    
    //message size
    memcpy(pm.buffer +offset,&payloadSize,sizeof(long));
    offset+= sizeof(long);
    
    //message payload
    memcpy(pm.buffer+offset, payload, payloadSize);
    
    return pm;
}
PackedMessage_t CreateMessageWithIntPayload(MessageType type, int payloadValue)
{
    //create a simple message with an integer payload
    return CreateMessage(type, (char*) &payloadValue, sizeof(int));
}
PackedMessage_t MemProtocol::CreateDataSetMessage(int error)
{
    return CreateMessageWithIntPayload(DataSet, error);
}
PackedMessage_t MemProtocol::CreateAccessDataMessage(int leaseId)
{
    return CreateMessageWithIntPayload(AccessData, leaseId);
}
PackedMessage_t MemProtocol::CreateLeasedMessage(int leaseId)
{
    return CreateMessageWithIntPayload(Leased, leaseId);
}
PackedMessage_t MemProtocol::CreateLeaseMessage(int leaseSize,int leaseDuration)
{
    char * payload = new char[sizeof(int)+sizeof(int)];
    
    memcpy(payload, &leaseSize, sizeof(int));
    memcpy(payload+sizeof(int), &leaseDuration, sizeof(int));
    return CreateMessage(Lease, payload, sizeof(int) + sizeof(int));
}
PackedMessage_t MemProtocol::CreateDataMessage(const char *buffer, long bufferSize, int leaseId, MessageType type)
{
    long size = sizeof(int) + sizeof(long) + bufferSize;
    char * payload = new char[size];
    int offset = 0 ;
    
    memcpy(payload,&leaseId,sizeof(int));
    offset+= sizeof(int);
    
    memcpy(payload +offset,&bufferSize,sizeof(long));
    offset += sizeof(long);
    
    memcpy(payload +offset,buffer,bufferSize);
    
    return CreateMessage(type, payload, size);
}
AccessDataMessage_t MemProtocol::ReadAccessMessage(PackedMessage_t message)
{
    int offset = MESSAGE_OVERHEAD;//skip type and size
    AccessDataMessage_t am;
    memcpy(&am.leaseId, message.buffer +offset, sizeof(int));
    return am;
}
LeaseMessage_t MemProtocol::ReadLeaseMessage(PackedMessage_t message)
{
    int offset = MESSAGE_OVERHEAD;//skip type and size
    LeaseMessage_t lm;
    memcpy(&lm.leaseSize, message.buffer +offset, sizeof(int));
    offset+= sizeof(int);
    memcpy(&lm.leasedDuration, message.buffer +offset, sizeof(int));
    return lm;
}
LeasedMessage_t MemProtocol::ReadLeasedMessage(PackedMessage_t message)
{
    int offset = MESSAGE_OVERHEAD;//skip type and size
    LeasedMessage_t lm;
    memcpy(&lm.leaseId, message.buffer +offset, sizeof(int));
    return lm;
}
DataSetMessage_t MemProtocol::ReadDataSetMessage(PackedMessage_t message)
{
    int offset = MESSAGE_OVERHEAD;//skip type and size
    DataSetMessage_t dm;
    memcpy(&dm.error, message.buffer +offset, sizeof(int));
    return dm;
}
DataMessage_t MemProtocol::ReadDataMessage(PackedMessage_t message)
{
    int offset = MESSAGE_OVERHEAD;//skip type
    DataMessage_t dm;
    
    //copy the packet
    memcpy(&dm.leaseId, message.buffer +offset, sizeof(int));
    offset += sizeof(int);
    
    memcpy(&dm.bufferSize, message.buffer +offset, sizeof(long));
    offset += sizeof(long);
    dm.buffer = NULL;
    if (dm.bufferSize >0)
    {
        dm.buffer = new char[dm.bufferSize];
        memcpy(dm.buffer, message.buffer +offset, dm.bufferSize);
    }
    return dm;
}