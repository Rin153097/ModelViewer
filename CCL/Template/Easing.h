#pragma once

#include <math.h>
#include "Util.h"

// Easing cheat sheat -> https://easings.net/ja

inline static float EaseOutCubic(float t) {
    return 1 - powf(1 - t, 3);
}

inline static float EaseOutCirc(float t) {
    return 1 - sqrtf(1 - powf(t - 1, 2));
}

inline static float EaseInSine(float t) {
    return 1 - cosf((t * 3.14159f) / 2);
}

inline static float EaseOutSine(float t) {
    return sinf((t * 3.14159f) / 2);
}

inline static float EaseInExpo(float t) {
    return Util::ApproxEqual(t, 0.0f) ? 0 : powf(2, 10 * t - 10);
}

inline static float EaseOutQuint(float t) {
    return 1.0f - powf(1.0f - t, 5.0f);
}

inline static float EaseInOutBack(float t) {
    const float c1 = 1.70158f;
    const float c2 = c1 * 1.525f;

    return t < 0.5f ?
        (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2 :
        (powf(2.0f * t - 2.0f, 2.0f) * (c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) / 2;

}