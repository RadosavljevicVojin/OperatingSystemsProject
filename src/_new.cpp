//
// Created by os on 8/4/23.
//

#include "../h/_new.hpp"
#include "../h/MemoryAllocatorr.hpp"

void* operator new(uint64 n){
    //Riscv::r_scause();
    //return __mem_alloc(n);
    size_t blocks = n / MEM_BLOCK_SIZE;
    blocks += (n % MEM_BLOCK_SIZE?1:0);
    return MemoryAllocatorr::getAllocator().allocate(blocks);
    //return __mem_alloc(n);
}

void* operator new[](uint64 n){
    //return __mem_alloc(n);
    size_t blocks = n / MEM_BLOCK_SIZE;
    blocks += (n % MEM_BLOCK_SIZE?1:0);
    return MemoryAllocatorr::getAllocator().allocate(blocks);
    //return __mem_alloc(n);
}
void operator delete(void* p) noexcept{
    //__mem_free(p);
    MemoryAllocatorr::getAllocator().deallocate(p);
}
void operator delete[](void* p) noexcept{
    //__mem_free(p);
    MemoryAllocatorr::getAllocator().deallocate(p);
}


