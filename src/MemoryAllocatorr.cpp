//
// Created by os on 8/19/23.
//
#include "../h/MemoryAllocatorr.hpp"

MemoryAllocatorr::MemoryAllocatorr() {
    head = (Node*)HEAP_START_ADDR;
    head->adr = (char*)HEAP_START_ADDR;
    head->size = ((uint64)HEAP_END_ADDR + 1-(uint64)HEAP_START_ADDR)/MEM_BLOCK_SIZE;
    head->next = nullptr;
}

void *MemoryAllocatorr::allocate(uint64 num) {
    Node* curr = head;
    Node* prev = nullptr;
    Node* fragment = nullptr;
    while(curr){
        if(curr->size >= num + 1){
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    if(!curr){
        return nullptr;
    }
    if(curr->size > num + 1){
        char* newAddress = curr->adr + (num + 1) * MEM_BLOCK_SIZE;
        fragment = (Node*)newAddress;
        fragment->adr = newAddress;
        fragment->size = curr->size - num - 1;
        fragment->next = curr->next;
    }else{
        return nullptr;
    }
    if(prev){
     if(fragment){
         prev->next = fragment;
     }else{
         prev->next = curr->next;
     }
    }else{
        if(fragment){
            head = fragment;
        }else{
            head = curr->next;
        }
    }
    curr->size = num + 1;
    char *ret = curr->adr + MEM_BLOCK_SIZE;
    return ret;
}

uint64 MemoryAllocatorr::deallocate(void *ptr) {
     Node* prev = nullptr;
     Node* curr = head;
     Node* segment = nullptr;
     uint64 flag = 0;
     while(curr){
         if(ptr < curr->adr && !prev){
             segment = (Node*)((char*)ptr - MEM_BLOCK_SIZE);
             segment->next = curr;
             head = segment;
             if(segment->adr + MEM_BLOCK_SIZE * segment->size == curr->adr){
                segment ->size += curr->size;
                segment->next = curr->next;
             }else{
                 segment->next = curr;
             }
             flag = 1;
             break;
         }else if(prev && ptr < curr->adr && ptr > prev->adr){
             segment = (Node*)((char*)ptr - MEM_BLOCK_SIZE);
             if(prev->adr + MEM_BLOCK_SIZE * prev->size == segment->adr){
                 prev->size += segment->size;
                 if(segment->adr + MEM_BLOCK_SIZE * segment->size == curr->adr){
                     prev->size += curr->size;
                     prev->next = curr->next;
                 }
             }else if(segment->adr + MEM_BLOCK_SIZE * segment->size == curr->adr){
                 segment->size += curr->size;
                 segment->next = curr->next;
                 prev->next = segment;
             }
             flag = 1;
             break;
         }
         prev = curr;
         curr = curr->next;
     }
     return flag;
}

