#pragma once

#include "../Vector.h"

static Vec2 CalcBezierCurve(const Vec2& s, const Vec2& c, const Vec2& e, const float t)
{
    Vec2 position = {};

    position.x =
        (1.0f - t) * (1.0f - t) * s.x +
        2.0f * (1.0f - t) * t * c.x +
        t * t * e.x;

    position.y =
        (1.0f - t) * (1.0f - t) * s.y +
        2.0f * (1.0f - t) * t * c.y +
        t * t * e.y;

    return position;
}