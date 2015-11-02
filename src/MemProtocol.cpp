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
    return CreateMessage(Lease, payload, sizeof(int));
}
PackedMessage_t MemProtocol::CreateDataMessage(const char *buffer, long bufferSize, int leaseId, MessageType type)
{
    long size = sizeof(long) + bufferSize;
    char * payload = new char[size];
    
    memcpy(payload,buffer,leaseId);
    memcpy(payload +sizeof(long),buffer,bufferSize);

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
    int offset = PACKET_SIZE_OFFSET;//skip type
    DataMessage_t dm;
    int size;
    
    //copy the packet
    memcpy(&dm.leaseId, message.buffer +offset, sizeof(int));
    offset += sizeof(int);
    
    memcpy(&size, message.buffer +offset, sizeof(int));
    dm.bufferSize = size - sizeof(int);
    offset += sizeof(int);
    
    memcpy(&dm.bufferSize, message.buffer +offset, dm.bufferSize);
    
    return dm;
}