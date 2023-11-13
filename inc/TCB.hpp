#ifndef tcb_hpp
#define tcb_hpp

#include "../lib/hw.h"
class Scheduler;
class Scheduler2;
class Riscv;
class TCB{
public:
    ~TCB(){
        delete[] stack;
    }
    bool isFinished(){
        return finished;
    }
    bool isSleeping() const{
        return sleeping;
    }
    void setFinished(){
        finished = true;
    }
    bool isBlocked() const{
        return blocked;
    }
    void setBlocked(){
        blocked = true;
    }
    void setUnblocked(){
        blocked = false;
    }
    void setSystemThread(){
        system = true;
    }
    void joinCurrentThread();
    static void yield();
    uint64 getTimeSlice() const{
        return timeSlice;
    }
    void setSleepingTime(time_t time){
        sleepingTime = time;
    }
    using Body = void (*)(void*);
    static TCB* running;
    static uint64 timeSliceCounter;
    static TCB* createThread(void* arg,Body body,uint64* stack);
    static int insertSleepingThread(time_t time);
    static void updateSleepingThreads();
    uint64 id = 0;
    //static void yield();
private:
//    struct Node{
//        TCB* elem;
//        Node* next;
//    };
    friend class Scheduler2;
    friend class _sem;
    static TCB* sleepingHead;
    static TCB* sleepingTail;
    TCB* nextSleepingThread = nullptr;
    TCB* nextOnScheduler = nullptr;
    TCB* nextOnSem = nullptr;
    TCB* joinHead = nullptr;
    TCB* joinTail = nullptr;
    TCB* nextOnJoin = nullptr;
    bool sleeping = false;
    TCB(void* arg,Body body,uint64* stack,uint64 timeSlice);
    static void dispatch();
    static int thread_exit();
    void(*fnc)(void*);
    bool blocked;
    //bool waitOnClosed = false;
    uint64 timeSlice;
    time_t sleepingTime;
    void* arg;
    volatile uint64 sp;
    volatile uint64 ra;
    volatile uint64* stack;
    volatile bool finished;
    bool system;
    friend class Riscv;
    static void Thread_wrapper();
    static void contextSwitch(TCB* old);
    static uint64 const TIME_SLICE = 2;

    void releaseJoinedThreads();
};

























#endif