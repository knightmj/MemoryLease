//
//  MemoryContainer.cpp
//  server
//
//  Created by Michael Knight on 10/31/15.
//  Copyright © 2015 Michael Knight. All rights reserved.
//

#include "LeasedMemoryContainer.hpp"
#include <math.h>

using namespace std;

long RoundUpToPowerOfTwo(long x)
{
    int power = 1;
    while(power < x)
        power*=2;
    return power;
}
long RoundDownToPowerOfTwo(long x)
{
    int power = 1;
    while(power < x)
        power*=2;
    return power/2;
}
//this method will recursively find a new buddy node
//if needed it will split the current node into two smaller
//group nodes
BuddyNode_t* FindBuddy(BuddyNode_t * cur, long size)
{
    //this node and children are allocated
    if (cur->allocated)
        return NULL;
    
    
    //has this node been split
    //if so keep searching left and right
    if (cur->IsSplit())
    {
        //we have already been split
        BuddyNode_t * n = FindBuddy(cur->left, size);
        if (n)
            return n;
        
        return FindBuddy(cur->right, size);
    }
    
    //this not has not been split
    //if it's the right size use it!
    else if (cur->size == size)
    {
        //this works
        cur->allocated = true;
        return cur;
    }
    //this node is not the right size.
    //can we split?
    else if (cur->size >= size*2)
    {
        //we need to split!
        cur->left = new BuddyNode_t(cur->memory,cur->size/2,false,cur);
        cur->right = new BuddyNode_t(cur->memory,cur->size/2,false,cur);
        
        return FindBuddy(cur->left, size);
    }
    
    //this block is too small to split, but also does not fit
    //so we can't find anything.
    return NULL;
}

//this method will search nodes upward from a newly freed node
//and combine any node who's chirlded are both now available.
void CompressUp(BuddyNode_t * node)
{
    BuddyNode_t * p = node->parent;
    if (!p)
        return;
    
    if (p->left->IsFree()  && p->right->IsFree())
    {
        delete p->left;
        delete p->right;
        p->left = NULL;
        p->right = NULL;
        p->allocated = false;
        CompressUp(p);
    }
}

LeasedMemoryContainer::LeasedMemoryContainer()
{
    srand( (unsigned int)time(NULL));
}

int LeasedMemoryContainer::CreateMemory(int maxMemory)
{
    //see if we can allocate something this large from the os
    this->memory = new char[maxMemory];
    if (!this->memory)
        return -1;
    
    this->maxMemory = maxMemory;
    this->head = new BuddyNode_t(this->memory,this->maxMemory,false,NULL);
    return 0;
}
int LeasedMemoryContainer::RequestLease(long size,
                                        int duration)
{
    // Get a lease identifer by getting a random number.
    // Given that we are not seeing the random number generator
    // it should provide a list of numbers that do not repeat until we frun 3^32 leases
    // Because of this, for now if we ever run into a situation where we have dupliate leases
    // we can simply return an error.
    int lease = rand();
    if (this->leaseLookup.find(lease) != this->leaseLookup.end())
        return -1;
    
    long neededSize = RoundUpToPowerOfTwo(size);
    
    neededSize = max(32l,neededSize); //smallest block of memory is 32 bytes
    
    BuddyNode_t * n =  FindBuddy(this->head, neededSize);
    
    //if we can not find a node then there is not enoug space
    if (!n)
        return -1;
    
    //set the needed info for this new lease
    this->leaseLookup[lease].size = size;
    this->leaseLookup[lease].leaseId = lease;
    this->leaseLookup[lease].start = time(NULL);
    this->leaseLookup[lease].node = n;
    this->leaseLookup[lease].duration = duration;
    
    return lease;
}
int LeasedMemoryContainer::SetMemory(char* mem,
                                     long size,
                                     int leaseId)
{
    //is this lease still active?
    if (this->leaseLookup.find(leaseId) == this->leaseLookup.end())
        return -1;
    
    //does this lease match the expected lease size?
    if (this->leaseLookup[leaseId].size != size)
        return -2;
    
    memcpy(this->leaseLookup[leaseId].node->memory, mem, size);
    return 0;
}

int LeasedMemoryContainer::GetMemory(char** mem,
                                     long &size,
                                     int leaseId)
{
    //is this lease still active?
    if (this->leaseLookup.find(leaseId) == this->leaseLookup.end())
        return -1;
    
    (*mem) = this->leaseLookup[leaseId].node->memory;
    size = this->leaseLookup[leaseId].size;
    
    return 0;
}

int LeasedMemoryContainer::CleanLeases()
{
    map<int,MemoryLease_t>::const_iterator it = this->leaseLookup.begin();
    vector<int> removedLeases;
    
    while (it != this->leaseLookup.end())
    {
        time_t end = it->second.start + it->second.duration;
        if (time(NULL) > end)
            removedLeases.push_back(it->first);
        it++;
    }
    //remove any old leases
    //and add there memory to the free list
    for (size_t i =0; i < removedLeases.size(); i++)
    {
        int lease = removedLeases[i];
        
        this->leaseLookup[lease].node->allocated = false;
        CompressUp(this->leaseLookup[lease].node);
        map<int,MemoryLease_t>::iterator it = this->leaseLookup.find(lease);
        this->leaseLookup.erase(it);
    }
    return -1;
}
