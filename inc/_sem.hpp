#ifndef _sem_hpp
#define _sem_hpp
#include "../lib/hw.h"
#include "syscall_c.hpp"
#include "../h/Scheduler2.hpp"
class _sem{
public:
    _sem(uint64 init){
        value = init;
        closed = false;
    };
    static _sem* createSem(uint64 value);


    bool  isClosed () const{
        return closed;
    }
    //uint64 sem_signal();
    //uint64 sem_wait();
    uint64 sem_signal();
    uint64 sem_wait();
private:
    bool closed;
    uint64 sem_close();
    friend class Riscv;
    int value;
//    struct Node{
//        TCB* elem;
//        Node* next;
//    };
     TCB* head = nullptr;
     TCB* tail = nullptr;
//    Node* head = nullptr;
//    Node* tail = nullptr;
    uint64 block();

    uint64 unblock();
};


#endif