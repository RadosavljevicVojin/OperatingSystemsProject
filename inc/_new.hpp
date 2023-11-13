#ifndef new_hpp
#define new_hpp

#include "../lib/hw.h"
void* operator new(uint64 n);

void* operator new[](uint64 n);
void operator delete(void* p) noexcept;
void operator delete[](void* p) noexcept;
#endif