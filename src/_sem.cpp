//
// Created by os on 8/4/23.
//
#include "../h/_sem.hpp"
#include "../test/printing.hpp"
#include "../h/_new.hpp"
_sem *_sem::createSem(uint64 value) {
    return new _sem(value);
}

uint64 _sem::sem_signal() {
    if(++value <= 0){
        return unblock();
    }
    return 0;
}

uint64 _sem::sem_wait() {
    if(--value < 0){
        return block();
    }
    return 0;
}

uint64 _sem::block() {
    //static int prom = 0;
    //volatile _sem* pomm = this;
    TCB* curr = TCB::running;
        if(!head){
        head = curr;
    }else{
        tail->nextOnSem = curr;
    }
        tail = curr;
        tail->nextOnSem = nullptr;
        curr->setBlocked();
        TCB::yield();
//    if((uint64)this == 0x04){
//        printString("Adresa je 0x03\n");
//    }
        if(closed)return -1;
       return 0;
}

uint64 _sem::unblock() {
    if(head){
        if(Scheduler2::put(head) < 0){
       return -1;
     }
        head->setUnblocked();
        TCB* curr = head;
        head = head->nextOnSem;
        if(head == nullptr){
            tail = nullptr;
        }
        curr->nextOnSem = nullptr;
    }
  return 0;
}

uint64 _sem::sem_close() {
    uint64 val;
    this->closed = true;
    while(value < 0){
        val = this->sem_signal();
        if(val < 0)return -1;
    }
    return 0;
}

