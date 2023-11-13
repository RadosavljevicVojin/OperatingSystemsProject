//
// Created by os on 8/17/23.
//
#include "../h/syscall_cpp.hpp"
#include "../h/_sem.hpp"
//void* operator new(size_t n) {
//    return __mem_alloc(n);
//}
//void* operator new[](size_t n) {
//    return __mem_alloc(n);
//}
//
//void operator delete(void* ptr) {
//    __mem_free(ptr);
//}
//
//void operator delete[](void* ptr) {
//    __mem_free(ptr);
//}


Thread::Thread(void (*body)(void*), void *arg) {
    this->body = body;
    this->arg = arg;
}

Thread::~Thread() {
     delete myHandle;
}

int Thread::start() {
    return thread_create(&myHandle,body,arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}


Thread::Thread() {
    this->body = RunThread;
    this->arg = this;
}

void Thread::join() {
    thread_join(this->myHandle);
}

void Thread::RunThread(void *thr) {
    ((Thread*)thr)->run();
}


Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle,init);
}

Semaphore::~Semaphore() {
    delete myHandle;
}

int Semaphore::wait() {
    if(myHandle != nullptr){
        return sem_wait(myHandle);
    }
    return -1;
}

int Semaphore::signal() {
    if(myHandle != nullptr){
        return sem_signal(myHandle);
    }
    return -1;
}



PeriodicThread::PeriodicThread(time_t period) {
    this->period = period;
}
void PeriodicThread::run() {
    while(!finished){
        periodicActivation();
        time_sleep(period);
    }
}

void PeriodicThread::terminate() {
    finished = true;
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}