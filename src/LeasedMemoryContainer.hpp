//
//  MemoryContainer.hpp
//  server
//
//  Created by Michael Knight on 10/31/15.
//  Copyright © 2015 Michael Knight. All rights reserved.
//

#ifndef MemoryContainer_hpp
#define MemoryContainer_hpp

#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <map>
#include <cstring>
// This class provides time based leases to a Buddy Allocator memory manager
// https://en.wikipedia.org/wiki/Buddy_memory_allocation
//
// Buddy Memory allocation provides flexable way to maintain a best-fit memory allocation
// with minimal overhead needed to account for fragmentation over time.



typedef std::map<long, std::vector<char*> *> FreeList_t;

typedef struct BuddyNode
{
    BuddyNode * left;
    BuddyNode * right;
    BuddyNode * parent;
    long size;
    bool allocated;
    char * memory;
    BuddyNode(char* memory,long size , bool allocated, BuddyNode * parent)
    {
        this->memory = memory;
        this->size = size;
        this->allocated = allocated;
        this->parent = parent;
    }
    bool IsSplit()
    {
        return this->left && this->right;
    }
    bool IsFree()
    {
        return !this->IsSplit() && !this->allocated;
    }
}BuddyNode_t;

typedef struct MemoryLease
{
    int leaseId;
    time_t start;
    int duration;
    long size;
    BuddyNode_t *node;
}MemoryLease_t;


class LeasedMemoryContainer
{
public:
    LeasedMemoryContainer();
    
    // create the memory used behind this application
    // memory size is in bytes.
    int CreateMemory(int maxMemory);
    
    // request a memory lease of a specific size
    // memory size is in bytes.
    // duration is in milliseconds
    int RequestLease(long size, int duration);
    
    //set the memory for the provided lease
    int SetMemory(char* mem, long size, int leaseId);
    
    //get the memory for given lease
    int GetMemory(char** mem, long &size, int leaseId);
    
    //remove any leases that are not longer active
    int CleanLeases();
private:
    std::map<int,MemoryLease_t> leaseLookup;
    BuddyNode_t * head;
    long maxMemory;
    char * memory;
};

#endif /* MemoryContainer_hpp */
