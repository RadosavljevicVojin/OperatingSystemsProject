//
// Created by os on 8/9/23.
//
#include "../h/BoundedBuffer.hpp"
#include "../h/_new.hpp"
#include "../h/TCB.hpp"

BoundedBuffer* BoundedBuffer::in = nullptr;
BoundedBuffer* BoundedBuffer::out = nullptr;
_sem* BoundedBuffer::outMutex = nullptr;
_sem* BoundedBuffer::inMutex = nullptr;
BoundedBuffer::BoundedBuffer() {
    mutex1 = new _sem(1);
    mutex2 = new _sem(1);
    spaceAvailable = new _sem(N);
    itemAvailable = new _sem(0);
    head = 0;
    tail = 0;
}

void BoundedBuffer::append(char c) {
    mutex1->sem_wait();
    spaceAvailable->sem_wait();
    buffer[tail] = c;
    tail = (tail + 1) % N;
    itemAvailable->sem_signal();
    mutex1->sem_signal();
}
void BoundedBuffer::appendSystem(char c) {
    sem_wait(mutex1);
    sem_wait(spaceAvailable);
    buffer[tail] = c;
    tail = (tail + 1) % N;
    sem_signal(itemAvailable);
    sem_signal(mutex1);
}
char BoundedBuffer::take() {
    mutex2->sem_wait();
    itemAvailable->sem_wait();
    char c = buffer[head];
    head = (head + 1) % N;
    spaceAvailable->sem_signal();
    mutex2->sem_signal();
    return c;
}
char BoundedBuffer::takeSystem() {
    sem_wait(mutex2);
    sem_wait(itemAvailable);
    char c = buffer[head];
    head = (head + 1) % N;
    sem_signal(spaceAvailable);
    sem_signal(mutex2);
    return c;
}


void BoundedBuffer::initBuffers() {
     out = new BoundedBuffer();
     out->id = 1;
     in = new BoundedBuffer();
     in->id = 2;
     inMutex = new _sem(0);
     outMutex = new _sem(0);
}
