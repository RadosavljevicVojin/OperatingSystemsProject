//
// Created by os on 8/14/23.
//
#include "../h/Scheduler2.hpp"
#include "../h/TCB.hpp"
TCB* Scheduler2::head = nullptr;
TCB* Scheduler2::tail = nullptr;
uint64 Scheduler2::put(TCB *old) {
    if(head == nullptr){
        head = old;
    }else{
        tail->nextOnScheduler = old;
    }
    tail = old;
    old->nextOnScheduler = nullptr;
    return 0;
}

TCB *Scheduler2::getNextTcb() {
    TCB* curr = head;
    head = head->nextOnScheduler;
    curr->nextOnScheduler = nullptr;
    if(head == nullptr){
        tail = nullptr;
    }
    return curr;
}

