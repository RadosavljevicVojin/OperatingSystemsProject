//
// Created by os on 7/1/23.
//
#include "../h/TCB.hpp"
#include "../h/Scheduler2.hpp"
#include "../h/syscall_c.hpp"
#include "../h/Riscv.h"
TCB* TCB::running = nullptr;
TCB* TCB::sleepingHead = nullptr;
TCB* TCB::sleepingTail = nullptr;
uint64 TCB::timeSliceCounter = 0;
extern "C" void pushRegisters();
extern "C" void popRegisters();
TCB::TCB(void* arg,Body body,uint64* stack,uint64 timeSlice){
    fnc = body;
    if(body != nullptr){
        this->stack = stack;
        sp = (uint64)stack + DEFAULT_STACK_SIZE;
        //sp = (uint64)&stack[DEFAULT_STACK_SIZE/8];
        ra = (uint64)&TCB::Thread_wrapper;
    }else{
        this->stack = nullptr;
        sp = 0;
        ra = 0;
    }
    this->arg = arg;
    this->timeSlice = timeSlice;
    sleeping = false;
    blocked = false;
    finished = false;
    system = false;
    if(body != nullptr){
        Scheduler2::put(this);
    }
}

void TCB:: dispatch(){
    TCB* curr;
     TCB* novi;
    curr = TCB::running;

    if(!curr->finished && !curr->isBlocked() && !curr->isSleeping()){
        Scheduler2::put(running);
    }
    //Riscv::r_scause();
    TCB* old = running;
    novi = Scheduler2::getNextTcb();
    running = novi;
//    printString("Iz schedulera je izvucen tcb sa id:");
//    printInteger(running->id);
//    printString("\n");
    TCB::contextSwitch(old);
}
 void TCB::Thread_wrapper() {
    Riscv::popSppSPie();
    running->fnc((void*)running->arg);
    running->finished = true;
    TCB::running->releaseJoinedThreads();
    ::thread_exit();
     thread_dispatch();
}
int TCB::thread_exit() {
    running->finished = true;
    delete running->stack;
    thread_dispatch();
    return 0;
}
void TCB::contextSwitch(TCB *old) {
    //u ovoj funkciji nema ugnjezdenih poziva tako da ce se iz nje vratiti na vrednost iz ra registra
    //kada nit prvi put dobija kontekst odmah ce odavde skociti u thread wrapper
    //kada drugi put dobije kontekst vratice se u dispatch ali ce onda preko steka procitati adresu dokle je stigla nit u thread wrapperu
    if(old){
        asm volatile("mv %[stariRa],ra":[stariRa]"=r"(old->ra));
        asm volatile("mv %[sp],sp":[sp]"=r"(old->sp):);
    }
    asm volatile("mv sp,%[sp]"::[sp]"r"(running->sp));
    //asm volatile("sd a2,-96(sp)");
    asm volatile("mv ra,%[noviRa]"::[noviRa]"r"(running->ra));
    asm volatile("mv sp,%[sp]"::[sp]"r"(running->sp));
//    printString("Iz schedulera je izvucen tcb sa id:");
//    printInteger(running->id);
//    printString("\n");
    //asm volatile("mv ra,%[noviRa]"::[noviRa]"r"(running->ra));

}
TCB* TCB::createThread( void *arg, TCB::Body body, uint64* stack) {
    return  new TCB(arg,body,stack,TIME_SLICE);

}

int TCB::insertSleepingThread(time_t time) {
    if(time == 0){
        Scheduler2::put(TCB::running);
        return 0;
    }
    //if(time < 0)return -1;
    TCB* curr = sleepingHead;
    TCB* novi = TCB::running;
    TCB* prev = nullptr;
    while(curr){
        if(time < curr->sleepingTime){
            novi->nextSleepingThread = curr;
            if(prev){
                prev->nextSleepingThread = novi;
            }else{
                sleepingHead = novi;
            }
            uint64 diff;
            if(prev){
                diff = time - prev->sleepingTime;
            }else
            {
                diff = time;
            }
            novi->nextSleepingThread->sleepingTime -= diff;
            novi->sleepingTime = time;
            novi->sleeping = true;
            return 0;
        }
        prev = curr;
        time -= curr->sleepingTime;
        curr = curr->nextSleepingThread;
    }
    if(sleepingTail != nullptr){
        sleepingTail->nextSleepingThread = novi;
    }else{
        sleepingHead = novi;
    }
    sleepingTail = novi;
    novi->sleeping = true;
    novi->sleepingTime = time;
    return 0;
}

void TCB::updateSleepingThreads() {
   if( sleepingHead && sleepingHead->sleepingTime == 1){
       TCB* curr = sleepingHead;
       curr->sleeping = false;
           TCB* pom = curr;
           curr = curr->nextSleepingThread;
           pom->nextSleepingThread = nullptr;
           Scheduler2::put(pom);
       while(curr && curr->sleepingTime == 0){
           curr->sleeping = false;
            pom = curr;
           curr = curr->nextSleepingThread;
           pom->nextSleepingThread = nullptr;
           Scheduler2::put(pom);
      }
       if(curr){
           //curr->sleepingTime -= 1;
           sleepingHead = curr;
           if(!curr->nextSleepingThread){
               sleepingTail = curr;
           }
       }else{
           sleepingHead = sleepingTail = nullptr;
       }
   }else{
       if(sleepingHead){
           sleepingHead->sleepingTime -= 1;
       }
   }
}

void TCB::yield() {
    TCB* novi = Scheduler2::getNextTcb();
    TCB* old = running;
    running = novi;
    pushRegisters();
    contextSwitch(old);
    popRegisters();
}

void TCB::joinCurrentThread() {
    if(!this->isFinished()){
        if(!joinHead){
            joinHead = TCB::running;
        }else{
            joinTail->nextOnJoin = TCB::running;
        }
        joinTail = TCB::running;
    }
}

void TCB::releaseJoinedThreads() {
       TCB* curr = joinHead;
       TCB* prev;
       while(curr){
           prev = curr;
           curr = curr->nextOnJoin;
           Scheduler2::put(prev);
           prev->nextOnJoin = nullptr;
       }
       joinHead = nullptr;
       joinTail = nullptr;
}
