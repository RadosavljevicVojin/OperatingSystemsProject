#ifndef MemoryAllocatorr_hpp
#define MemoryAllocatorr_hpp

#include "../lib/hw.h"

class MemoryAllocatorr{
public:
   static MemoryAllocatorr& getAllocator(){
       static MemoryAllocatorr MemoryAllocator;
       return MemoryAllocator;
   }
    MemoryAllocatorr(const MemoryAllocatorr&) = delete;
    MemoryAllocatorr& operator=(const MemoryAllocatorr&) = delete;
    void* allocate(uint64 num);
    uint64 deallocate(void* ptr);

private:
    struct Node{
        char* adr;
        uint64 size;
        Node* next;
    };
   MemoryAllocatorr();
   Node* head;
};


#endif