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

// This class provides time based leases to a Buddy Allocator memory manager
// https://en.wikipedia.org/wiki/Buddy_memory_allocation
// Buddy Memory allocation provides flexible way to maintain a best-fit memory allocation
// with minimal overhead needed to account for fragmentation over time.

typedef std::map<long, std::vector<char*> *> FreeList_t;

//this is a buddy node that contains links to two children
//and the parent
typedef struct BuddyNode
{
    BuddyNode * left;//left buddy
    BuddyNode * right;//right buddy
    BuddyNode * parent;//parent of this node
    long size; //size of memory here
    bool allocated; //is this memory allocated?
    char * memory; // memory to allocate
    BuddyNode(char* memory,long size , bool allocated, BuddyNode * parent)
    {
        this->memory = memory;
        this->size = size;
        this->allocated = allocated;
        this->parent = parent;
        this->left = NULL;
        this->right = NULL;
    }
    //was this node split?
    bool IsSplit()
    {
        return this->left && this->right;
    }
    //is this node allocated?
    bool IsFree()
    {
        return !this->IsSplit() && !this->allocated;
    }
}BuddyNode_t;

typedef struct MemoryLease
{
    int leaseId; //id of the lease
    time_t start;//time the lease started
    int duration;//duration of the lease
    long size; //size of the memory leased
    BuddyNode_t *node; //the buddy node that was allocated for the lease
}MemoryLease_t;


class LeasedMemoryContainer
{
public:
    LeasedMemoryContainer();
    
    // create the memory used behind this application
    // maxMemory: size of the memory poll to lease in bytes.
    // returns: 
    //             0 if successful
    //            -1 if we fail to allocate memory
    int CreateMemory(int maxMemory);
    
    // request a memory lease of a specific size
    // size: memory size is in bytes.
    // duration: duration  of lease is in milliseconds
    // returns: 
    //             0 if successful
    //            -1 if we fail create a lease due to lack of resources
    int RequestLease(long size, int duration);
    
    //set the memory for the provided lease
    // mem: memory to set
    // size: size of the memory buffer to set
    // leaseId: id of the needed lease
    // returns: 
    //             0 if successful
    //            -1 if we can not find this lease
    int SetMemory(char* mem, long size, int leaseId);
    
    //get the memory for given lease
    // mem: memory pointer to get
    // size: size of the memory buffer that was gotten
    // leaseId: id of the needed lease
    // returns: 
    //             0 if successful
    //            -1 if we can not find this lease
    int GetMemory(char** mem, long &size, int leaseId);
    
    //remove any leases that are not longer active
    // returns: 
    //             0 if successful
    int CleanLeases();
private:
    std::map<int,MemoryLease_t> leaseLookup;
    BuddyNode_t * head;
    long maxMemory;
    char * memory;
};

#endif /* MemoryContainer_hpp */
