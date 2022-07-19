#pragma once

#include <float.h>

inline static bool ApproxEqual(float lhs, float rhs, float epsilon = FLT_EPSILON) {
    return lhs > rhs - epsilon && lhs < rhs + epsilon;
}

template <class T>
static void SafeDelete(T*& p) {
    if (p) {
        delete p;
        p = nullptr;
    }
}