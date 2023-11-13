//
// Created by os on 5/17/23.
//
#include "../h/syscall_c.hpp"
#include "../h/Riscv.h"
//#include "../h/userMain.hpp"
#include "../h/BoundedBuffer.hpp"
#include "../test/printing.hpp"
#include "../h/syscall_cpp.hpp"
extern "C" void supervisorTrap();
extern void userMain();
void idle(void* arg){
    while(true){
        thread_dispatch();
    }
};
void testMain() {
    for (int i = 0; i < 1000; i++) {
        printInt(i);
        putc('\n');
    }
    //time_sleep(20);
}
void userMainWrapper(void* arg){
    userMain();
}
class MyThread:public PeriodicThread{
public:
    MyThread(time_t period): PeriodicThread(period){

    }
    void periodicActivation() override{
        putc('a');
    }
};
void main() {
   asm volatile("csrw stvec,%[vector]": :[vector]"r"((uint64)&supervisorTrap | 1UL));
    //asm volatile("csrw stvec,%[vector]": :[vector]"r"(&Trap));
    thread_t mainThread = nullptr;
    thread_create(&mainThread,nullptr,nullptr);
    TCB::running = mainThread;
    thread_t  idleThread = nullptr;
    thread_create(&idleThread,idle,nullptr);
    thread_create(&Riscv::kernelThreadOut,Riscv::kernelThreadOutt,nullptr);
   thread_create(&Riscv::kernelThreadIn,Riscv::kernelThreadInn,nullptr);
   Riscv::kernelThreadOut->setSystemThread();
    Riscv::kernelThreadIn->setSystemThread();
    BoundedBuffer::initBuffers();
    //Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
//    Thread* period = new MyThread(10);
//    period->start();
//    period->join();
    thread_t userMainThread = nullptr;
    thread_create(&userMainThread,userMainWrapper,nullptr);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
    while(!userMainThread->isFinished()){
        thread_dispatch();
    }
    thread_dispatch();

}

