#ifndef BoundedBuffer_HPP
#define BoundedBuffer_HPP
#include "../h/_sem.hpp"
const int N = 256;
class BoundedBuffer{
public:
    BoundedBuffer();
    void append(char);
    char take();
    void appendSystem(char);
    char takeSystem();
    static BoundedBuffer* out;
    static BoundedBuffer* in;
    static _sem* outMutex;
    static _sem* inMutex;
    static void initBuffers();
    uint64 id = 0;
private:
    friend class Riscv;
    _sem* mutex1;
    _sem* mutex2;
    _sem* spaceAvailable;
    _sem* itemAvailable;
    char buffer[N];
    int head,tail;
};

#endif