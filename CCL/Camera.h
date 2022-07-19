#pragma once

#include "Vector.h"

class CameraBase {
public:
    float fovAngle;
    float aspect;
    float nearZ;
    float farZ;

private:
    Matrix projection;
    Matrix view;

    Matrix viewProjection;

public:
    CameraBase() { AllReset(); }
public:
    virtual void AllReset() {
        fovAngle = {};
        aspect = {};
        nearZ = {};
        farZ = {};

        projection = {};
        view = {};
    }

    virtual DirectX::XMMATRIX MakeView() = 0;

    void UpdateProjection();
    void UpdateView();
    void UpdateViewProjection();


    void UpdateAllCameraMatrix();

    Matrix GetProjectionMatrix();
    Matrix GetInverseView();
    Matrix GetViewMatrix();
    Matrix GetViewProjectionMatrix();

    Vec3 GetCameraFront();
    Vec3 GetCameraUp();
    Vec3 GetCameraRight();
    Vec3 GetCameraPos();
};

class CameraLookAt : public CameraBase {
public:
    Vec3 eye;
    Vec3 focus;
    Vec3 up;
public:
    void AllReset() override {
        CameraBase::AllReset();
        eye = {};
        focus = {};
        up = {};
    }

    DirectX::XMMATRIX MakeView() override;
};

class CameraUseAngle : public CameraLookAt {
public:
    Vec3 angle;
    float range;
public:
    void UpdateUseImGui();
    void Update();
    void AllReset() override {
        CameraLookAt::AllReset();
        angle = {};
    }

    DirectX::XMMATRIX MakeView() override;
};

class CameraController {
    CameraUseAngle* camera;

    float velocity;

public:
    CameraController(CameraUseAngle* camera) : camera(camera), velocity() {}

public:
    void SetCamera(CameraUseAngle* camera) { this->camera = camera; }
    void Update();
};