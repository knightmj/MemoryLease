//
//  MemProtocol.hpp
//  server
//
//  Created by Michael Knight on 10/31/15.
//  Copyright © 2015 Michael Knight. All rights reserved.
//

#ifndef MemProtocol_hpp
#define MemProtocol_hpp

#define MESSAGE_OVERHEAD sizeof(int) + sizeof(long)
#define PACKET_SIZE_OFFSET sizeof(int)

#include <stdio.h>
#include <cstring>

typedef struct LeasedMessage
{
    int leaseId;
}LeasedMessage_t;


typedef struct DataSetMessage
{
    int error;
}DataSetMessage_t;

typedef struct AccessDataMessage
{
    int leaseId;
}AccessDataMessage_t;

typedef struct LeaseMessage
{
    int leaseSize;
    int leasedDuration;
}LeaseMessage_t;

typedef struct DataMessage
{
    int leaseId;
    char * buffer;
    long bufferSize;
}DataMessage_t;

typedef struct PackedMessage
{
    long messageSize;
    long bufferSize;
    char * buffer;
    PackedMessage()
    {
        messageSize =0;
        buffer = NULL;
    }
    int Type()
    {
        return (int) (*buffer);
    }
    int Size()
    {
        return (*(buffer+PACKET_SIZE_OFFSET));
    }
    void AppendData(char * data, long size)
    {
        if (buffer == NULL)
        {
            buffer = new char[size];
            bufferSize = size;
        }
        else if (bufferSize < messageSize+size)
        {
            //do we have the full message size
            if (bufferSize + size > MESSAGE_OVERHEAD)
            {
                bufferSize = Size() + MESSAGE_OVERHEAD; //size of the packet plus the overhead
            }
            else
            {
                //grow the buffer to fit just the new data
                bufferSize = bufferSize + size;
            }
            char * newBuffer = new char[bufferSize];
            memcpy(newBuffer, buffer, bufferSize - size);
            delete [] buffer;
            buffer= newBuffer;
        }
        memcpy(buffer + messageSize, data, size);
        messageSize+=size;
    }
    bool Complete()
    {
        return buffer && Size()+ MESSAGE_OVERHEAD >= messageSize;
    }
}PackedMessage_t;
enum MessageType
{
    Lease = 1, //request a lease
    Leased, //lease requested
    SetData, //set the data
    AccessData, //access the data
    DataSet, //data has been set
};
class MemProtocol
{
public:
    static PackedMessage_t CreateLeaseMessage(int leaseSize,
                                              int leaseDuration);
    
    static PackedMessage_t CreateLeasedMessage(int leaseId);
    static PackedMessage_t CreateAccessDataMessage(int leaseId);
    static PackedMessage_t CreateDataSetMessage(int error);
    
    static PackedMessage_t CreateDataMessage(const char * buffer,
                                             long bufferSize,
                                             int leaseId,
                                             MessageType type);
    
    static LeaseMessage_t ReadLeaseMessage(PackedMessage_t message);
    static AccessDataMessage_t ReadAccessMessage(PackedMessage_t message);
    static LeasedMessage_t ReadLeasedMessage(PackedMessage_t message);
    static DataMessage_t ReadDataMessage(PackedMessage_t message);
    static DataSetMessage_t ReadDataSetMessage(PackedMessage_t message);
};


#endif /* MemProtocol_hpp */
