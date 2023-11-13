//
// Created by os on 7/13/23.
//
#include "../h/Riscv.h"
#include "../h/TCB.hpp"
#include "../h/_sem.hpp"
#include "../h/BoundedBuffer.hpp"
#include "../h/MemoryAllocatorr.hpp"

TCB* Riscv::kernelThreadIn = nullptr;
 TCB* Riscv::kernelThreadOut = nullptr;
void Riscv::kernelThreadInn(void* arg){
    while(true){
        uint8 broj = *((uint8*)CONSOLE_STATUS);
        char c;
        if(broj & CONSOLE_RX_STATUS_BIT){
            c  = *((char*)CONSOLE_RX_DATA);
            //BoundedBuffer::in->append(c);
            BoundedBuffer::in->appendSystem(c);
        }else{
            //BoundedBuffer::inMutex->sem_wait();
            sem_wait(BoundedBuffer::inMutex);
        }
    }
}
void Riscv::kernelThreadOutt(void* arg){
    while(true){
        uint8 broj = *((uint8*)CONSOLE_STATUS);
        char c;
        if(broj & CONSOLE_TX_STATUS_BIT){
            //c = BoundedBuffer::out->take();
            c = BoundedBuffer::out->takeSystem();
            *((uint8*)CONSOLE_TX_DATA) = c;
        }else{
            //BoundedBuffer::outMutex->sem_wait();
            sem_wait(BoundedBuffer::outMutex);
        }
    }
}
void Riscv::popSppSPie() {
    asm volatile("csrw sepc,ra");
    if(!TCB::running->system){
        Riscv::mc_sstatus(SSTATUS_SPP);
    }else{
        Riscv::ms_sstatus(SSTATUS_SPP);
        //Riscv::ms_sstatus(SSTATUS_SPIE);
    }
    asm volatile("sret");
}
void Riscv::HandleTrap(){
        uint64 volatile scause;
        uint64 volatile sepc;
        uint64 volatile sstatus;
        uint64 volatile code;
        uint64 volatile arg0;
        uint64 volatile arg1;
        uint64 volatile arg2;
        uint64 volatile arg3;
        uint64 volatile arg4;
        uint64 volatile value;
        asm volatile("mv %[arg4],a4":[arg4]"=r"(arg4):);
        asm volatile("mv %[arg3],a3":[arg3]"=r"(arg3):);
        asm volatile("mv %[arg2],a2":[arg2]"=r"(arg2):);
        asm volatile("mv %[arg1],a1":[arg1]"=r"(arg1):);
        asm volatile("mv %[arg0],a0":[arg0]"=r"(arg0):);
        asm volatile("mv %[code],a0":[code]"=r"(code):);
        scause = r_scause();
        sepc = r_sepc();
        sstatus = r_sstatus();
        if(scause == 0x0000000000000008 || scause == 0x0000000000000009){

            switch(code){
                case 0x01:
                    //mem_alloc
                    void* address;
                    uint64 blocks;
                    asm volatile("mv %[blocks],a1":[blocks]"=r"(blocks):);
                    address = MemoryAllocatorr::getAllocator().allocate(blocks);
                    __asm__ volatile("mv a0,%[address]": :[address]"r"(address));
                    asm volatile("sd a0, 10 * 8(fp)");
                    break;
                case 0x02:
                    //mem_free
                    void* ptr;
                    asm volatile("mv %[ptr],a1":[ptr]"=r"(ptr):);
                    value = MemoryAllocatorr::getAllocator().deallocate(ptr);
                    __asm__ volatile("mv a0,%[value]": :[value]"r"(value));
                    asm volatile("sd a0, 10 * 8(fp)");
                    break;
                case 0x11:
                    //thread_create
                     uint64* sp;
                    void* arg;
                    void(*fncPtr)(void*);
                    TCB** handle;
                    sp = (uint64*)arg4;
                    arg = (void*)arg3;
                    fncPtr = (void(*)(void*))arg2;
                    handle = ( TCB**)arg1;
                    *handle = TCB::createThread(arg,fncPtr,sp);
                    if(*handle == nullptr){
                        value = -1;
                    }else{
                        value = 0;
                    }
                    __asm__ volatile("mv a0,%[value]": :[value]"r"(value));
                    asm volatile("sd a0, 10 * 8(fp)");
                    break;
                case 0x12:
                    //thread_exit
                    TCB::thread_exit();
                case 0x13:
                    //thread_dispatch
                    TCB::dispatch();
                    break;
                case 0x14:
                    //thread_join
                    TCB* joinThread;
                    joinThread = (TCB*)arg1;
                    joinThread->joinCurrentThread();
                    if(!joinThread->isFinished()){
                        TCB::yield();
                    }
                case 0x21:
                    //sem_open
                    _sem** hand;
                    hand = (_sem**)arg1;
                    value = (uint64)arg2;
                    *hand = _sem::createSem(value);
                    if(*hand == nullptr){
                        value = -1;
                    }else{
                        value = 0;
                    }
                    __asm__ volatile("mv a0,%[value]": :[value]"r"(value));
                    asm volatile("sd a0, 10 * 8(fp)");
                    break;
                case 0x22:
                    //sem_close
                    _sem* id;
                    id = (_sem*)arg1;
                    value = (id)->sem_close();
                    __asm__ volatile("mv a0,%[value]": :[value]"r"(value));
                    asm volatile("sd a0, 10 * 8(fp)");
                    break;
                case 0x23:
                    //sem_wait
                    _sem* rucka;
                    rucka = (_sem*)arg1;
                    if(rucka->isClosed()){
                        value = -1;
                    }else{
                        value = rucka->sem_wait();
                    }
                    __asm__ volatile("mv a0,%[value]": :[value]"r"(value));
                    asm volatile("sd a0, 10 * 8(fp)");
                    break;
                case 0x24:
                    //sem_signal
                    _sem* r;
                    r = (_sem*)arg1;
                    value = (r)->sem_signal();
                    __asm__ volatile("mv a0,%[value]": :[value]"r"(value));
                    asm volatile("sd a0, 10 * 8(fp)");
                    break;
                case 0x31:
                    //time_sleep
                    volatile uint64 time;
                    time = (uint64)arg1;
                    value = TCB::insertSleepingThread(time);
                    TCB::yield();
                    __asm__ volatile("mv a0,%[value]": :[value]"r"(value));
                    asm volatile("sd a0, 10 * 8(fp)");
                    break;
                case 0x42:
                    //putc
                    char c;
                    c = (char)arg1;
                    BoundedBuffer::out->append(c);
                    break;
                case 0x41:
                    //getc
                    value = (uint64)BoundedBuffer::in->take();
                    __asm__ volatile("mv a0,%[value]": :[value]"r"(value));
                    asm volatile("sd a0, 10 * 8(fp)");
                    break;
                default:
                    break;
            }
            sepc += 4;
            w_sepc(sepc);
            w_sstatus(sstatus);
        }else{

            //putc('G');
        }
    }
void Riscv::HandleTimer() {
    uint64 volatile sepc;
    uint64 volatile sstatus;
    sepc = r_sepc();
    sstatus = r_sstatus();
    mc_sip(SIP_SSIP);
    TCB::updateSleepingThreads();
    TCB::timeSliceCounter++;
    if(TCB::timeSliceCounter >= TCB::running->getTimeSlice()){
        sepc = r_sepc();
        sstatus = r_sstatus();
        TCB::timeSliceCounter = 0;
        TCB::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
}

void Riscv::HandleConsole() {
    uint64 volatile sepc;
    uint64 volatile sstatus;
    sepc = r_sepc();
    sstatus = r_sstatus();
    int code = plic_claim();
    if (code == CONSOLE_IRQ) {
        mc_sip(SIP_SEIP);
        plic_complete(code);
        if (Riscv::kernelThreadOut->isBlocked()) {
            BoundedBuffer::outMutex->sem_signal();
        }
        if (Riscv::kernelThreadIn->isBlocked()) {
            BoundedBuffer::inMutex->sem_signal();
        }
    }
    w_sstatus(sstatus);
    w_sepc(sepc);
}
 uint64 Riscv::r_scause() {
    uint64 volatile scause;
    asm volatile("csrr %[scause],scause":[scause]"=r"(scause));
    return scause;
}

 void Riscv::w_scause(uint64 scause) {
    asm volatile("csrw scause,%[scause]"::[scause] "r"(scause));
}

 uint64 Riscv::r_sepc() {
    uint64 volatile sepc;
    asm volatile("csrr %[sepc],sepc":[sepc]"=r"(sepc));
    return sepc;
}

 void Riscv::w_sepc(uint64 sepc) {
    asm volatile("csrw sepc,%[sepc]"::[sepc] "r"(sepc));
}

 uint64 Riscv::r_stvec() {
    uint64 volatile stvec;
    asm volatile("csrr %[stvec],stvec":[stvec]"=r"(stvec));
    return stvec;
}

 void Riscv::w_stvec(uint64 stvec) {

}

 uint64 Riscv::r_stval() {
    return 0;
}

 void Riscv::w_stval(uint64 stval) {
    asm volatile("csrw stval,%[stval]"::[stval] "r"(stval));
}

 void Riscv::ms_sstatus(uint64 mask) {
    asm volatile("csrs sstatus,%[mask]"::[mask] "r"(mask));
}

void Riscv::mc_sstatus(uint64 mask) {
    asm volatile("csrc sstatus,%[mask]"::[mask] "r"(mask));
}

uint64 Riscv::r_sstatus() {
    uint64 volatile sstatus;
    //sstatus = 2;
    //asm volatile("csrr %0,sstatus":"=r"(sstatus));
    asm volatile("csrr %[sstatus],sstatus":[sstatus]"=r"(sstatus));
    return sstatus;
}

 void Riscv::w_sstatus(uint64 sstatus) {
    asm volatile("csrw sstatus,%[sstatus]"::[sstatus] "r"(sstatus));
}

 void Riscv::ms_sip(uint64 mask) {
    asm volatile("csrs sip,%[mask]"::[mask] "r"(mask));
}

void Riscv::mc_sip(uint64 mask) {
    asm volatile("csrc sip,%[mask]"::[mask] "r"(mask));
}



inline uint64 Riscv::r_sip() {
    uint64 volatile sip;
    asm volatile("csrr %[sip],sip":[sip]"=r"(sip));
    return sip;
}

inline void Riscv::w_sip(uint64 sip) {
    asm volatile("csrw sip,%[sip]"::[sip] "r"(sip));
}
