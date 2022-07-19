#pragma once

#include "Vector.h"

class DirectionalLight {
private:
    Vec3 direct;
public:
    Vec3 angle;

public:
    Vec3 GetLightDirect() { return direct; }
    void UpdateLightDirect();
    void UpdateUseImGui();

    void Update();
};