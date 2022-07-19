#pragma once

template <class T>
inline T clamp(T num, T lo, T hi) { return (std::max)((std::min)(num, hi), lo); }

template <class T>
inline T lerp(T start, T end, float factor) {
    return start + factor * (end - start);
}

template <class T>
inline T ReverseSign(T num, T reverseCenter = {}) {
    T sub = reverseCenter - num;
    return num + (2.0f * sub);
}
template <class T>
inline T RandomFromTo(T from, T to) {
    float factor = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return lerp(from, to, factor);
}

#define PI          (3.1415926535897932384626433832795f)
#define PIx2        (6.283185307179586476925286766559f)
#define PIdiv180    (0.01745329251994329576923690768489f)
#define PIdiv2      (1.5707963267948966192313216916398f)
#define PIdiv3      (1.0471975511965977461542144610932f)
#define PIdiv4      (0.78539816339744830961566084581988f)
#define PIdiv6      (0.52359877559829887307710723054658f)

#define Deg360      PIx2
#define Deg270      (4.7123889803846898576939650749193f)
#define Deg180      PI
#define Deg1        PIdiv180
#define Deg90       PIdiv2
#define Deg60       PIdiv3
#define Deg45       PIdiv4
#define Deg30       PIdiv6

#define ToRad(x)    (PIdiv180 * x)
#define ToDeg(x)    (x / PIdiv180)