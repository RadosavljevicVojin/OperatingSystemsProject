//
// Created by os on 7/13/23.
//

#ifndef PROJEKAT_OSS_RISCV_H
#define PROJEKAT_OSS_RISCV_H
#include "../lib/hw.h"
extern "C" void HandleTrap();
class TCB;
class Riscv
{
public:
    //static void Trap();
    //pop sstatus.spp and sstatus.spie bits(has to be a non inline function)
    static void popSppSPie();

    //read register scause
    static uint64 r_scause();

    //write register scause
    static void w_scause(uint64 scause);

    static uint64 r_sepc();
    static void w_sepc(uint64 sepc);
    //read register stvec
    static uint64 r_stvec();

    //write register stvec
    static void w_stvec(uint64 stvec);

    //read register stval
    static uint64 r_stval();

    //write register stval
    static void w_stval(uint64 stval);

    enum BitMaskSip
    {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };
    //mask set register sip
    static void ms_sip(uint64 mask);

    //mask clear register sip
    static void mc_sip(uint64 mask);

    //read register sip
    static uint64 r_sip();

    //write register sip
    static void w_sip(uint64 sip);

    enum BitMaskSstatus{
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };
    //mask set register sstatus
    static void ms_sstatus(uint64 mask);

    //mask clear register sstatus
    static void mc_sstatus(uint64 mask);

    //read register sstatus
    static uint64 r_sstatus();

    //write register sstatus
    static void w_sstatus(uint64 sstatus);
    static TCB* kernelThreadIn;
    static TCB* kernelThreadOut;
    //static void Trap();
    static void kernelThreadInn(void* arg);
    static void kernelThreadOutt(void* arg);
private:
    static void HandleTrap();
    static void HandleTimer();
    static void HandleConsole();
};


/*inline uint64 Riscv::r_sstatus() {
    uint64 volatile sstatus;
    //sstatus = 2;
    //asm volatile("csrr %0,sstatus":"=r"(sstatus));
    asm volatile("csrr %[sstatus],sstatus":[sstatus]"=r"(sstatus));
    return sstatus;
}*/

#endif //PROJEKAT_OSS_RISCV_H
