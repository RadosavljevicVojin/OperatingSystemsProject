#ifndef scheduler2_hpp
#define scheduler2_hpp
#include "../h/_new.hpp"

class TCB;
class Scheduler2{
public:
    static TCB* getNextTcb();
    static uint64 put( TCB* old);
    static TCB* head;
private:
    //static TCB* head;
    static TCB* tail;

};

#endif