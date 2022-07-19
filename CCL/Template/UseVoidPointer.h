#pragma once

#include <crtdbg.h>

template<class T>
void ConvertVoidP(void* vP, T* yourSelfP) {
    *yourSelfP = *(T*)vP;
}

