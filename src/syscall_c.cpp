//
// Created by os on 6/29/23.
//
#include "../lib/hw.h"
#include "../h/syscall_c.hpp"
void* mem_alloc(size_t size){
    size_t code = 0x01;
    void* adr;
    adr = nullptr;
    //return adr;
  size_t blocks = size / MEM_BLOCK_SIZE;
  blocks += (size % MEM_BLOCK_SIZE?1:0);
  asm volatile("mv a1,%[blocks]": :[blocks]"r"(blocks));
  __asm__ volatile("mv a0,%[code]": :[code]"r"(code));
  __asm__ volatile("ecall");
  asm volatile("mv %[adr],a0":[adr]"=r"(adr):);
  return adr;
}
void thread_dispatch(){
     size_t code = 0x13;
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
}
int thread_exit(){
    volatile size_t code = 0x12;
    //volatile uint64 value;
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
    return -2;
}
int mem_free(void* ptr){
    size_t code = 0x02;
    uint64 value;
    asm volatile("mv a1,%[ptr]": :[ptr]"r"(ptr));
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
    asm volatile("mv %[value],a0":[value]"=r"(value):);
    return value;
}
int thread_create(TCB** handle,void(*start_routine)(void*),void* arg){
    size_t code = 0x11;
    //void* sp = mem_alloc(DEFAULT_STACK_SIZE);
    void* sp;
    if(start_routine != nullptr) {
        //uint64* stack;
        //stack = new uint64 [512];
        //sp = __mem_alloc(DEFAULT_STACK_SIZE);
        //sp = (void*)&stack[512];
        sp = (char *) mem_alloc(DEFAULT_STACK_SIZE);
    }else{
        sp = nullptr;
    }
    uint64 value;
    asm volatile("mv a4,%[sp]"::[sp]"r"(sp));
    asm volatile("mv a3,%[arg]"::[arg]"r"(arg));
    asm volatile("mv a2,%[start]"::[start]"r"(start_routine));
    asm volatile("mv a1,%[handle]"::[handle]"r"(handle));
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
    asm volatile("mv %[value],a0":[value]"=r"(value):);
    return value;
}
int sem_open(sem_t* handle,unsigned init){
    size_t code = 0x21;
    uint64 value;
    asm volatile("mv a2,%[init]"::[init]"r"(init));
    asm volatile("mv a1,%[handle]"::[handle]"r"(handle));
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
    asm volatile("mv %[value],a0":[value]"=r"(value):);
    return value;
}
int sem_close(sem_t handle){
    size_t code = 0x22;
    uint64 value;
    asm volatile("mv a1,%[handle]"::[handle]"r"(handle));
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
    asm volatile("mv %[value],a0":[value]"=r"(value):);
    return value;
}
int sem_wait(sem_t id){
    size_t code = 0x23;
    uint64 value;
    asm volatile("mv a1,%[id]"::[id]"r"(id));
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
    asm volatile("mv %[value],a0":[value]"=r"(value):);
    return value;
}
int sem_signal(sem_t id){
    size_t code = 0x24;
    uint64 value;
    asm volatile("mv a1,%[id]"::[id]"r"(id));
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
    asm volatile("mv %[value],a0":[value]"=r"(value):);
    return value;
}
int time_sleep(time_t time){
    size_t code = 0x31;
    uint64 value;
    asm volatile("mv a1,%[time]"::[time]"r"(time));
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
    asm volatile("mv %[value],a0":[value]"=r"(value):);
    return value;
}
void thread_join(thread_t handle){
    size_t code = 0x14;
    asm volatile("mv a1,%[handle]"::[handle]"r"(handle));
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
}
char getc(){
    size_t code = 0x41;
    char value;
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
    asm volatile("mv %[value],a0":[value]"=r"(value):);
    return value;
}

void putc(char c){
    size_t code = 0x42;
    asm volatile("mv a1,%[c]"::[c]"r"(c));
    asm volatile("mv a0,%[code]"::[code]"r"(code));
    asm volatile("ecall");
}