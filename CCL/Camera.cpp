#include "framework.h"
#include "CCL.h"

#include "Camera.h"

#include "InputPrimitive.h"

#include "Template/inlines.h"
#include "Template/inlines.h"

void CameraBase::UpdateProjection() {
    using namespace DirectX;
    XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovAngle, aspect, nearZ, farZ);
    XMStoreFloat4x4(&projection, Projection);
}
void CameraBase::UpdateView() {
    using namespace DirectX;
    XMMATRIX View = MakeView();
    XMStoreFloat4x4(&view, View);
}
void CameraBase::UpdateViewProjection() {
    using namespace DirectX;
    XMMATRIX ViewProjection = XMLoadFloat4x4(&view) * XMLoadFloat4x4(&projection);
    XMStoreFloat4x4(&viewProjection, ViewProjection);
}
void CameraBase::UpdateAllCameraMatrix() {
    using namespace DirectX;
    UpdateProjection();
    UpdateView();

    UpdateViewProjection();
}

Matrix CameraBase::GetProjectionMatrix() {
    using namespace DirectX;
    return projection;
}
Matrix CameraBase::GetInverseView() {
    using namespace DirectX;
    Matrix r;
    XMMATRIX IV = DirectX::XMMatrixInverse(nullptr, XMLoadFloat4x4(&view));
    XMStoreFloat4x4(&r, IV);
    return r;
}
Matrix CameraBase::GetViewMatrix() {
    using namespace DirectX;
    return view;
}
Matrix CameraBase::GetViewProjectionMatrix() {
    return viewProjection;
}

Vec3 CameraBase::GetCameraRight() {
    using namespace DirectX;
    Matrix iV = this->GetInverseView();
    return Vec3(iV.m[0][0], iV.m[0][1], iV.m[0][2]).Normalize();
}
Vec3 CameraBase::GetCameraUp() {
    using namespace DirectX;
    Matrix iV = this->GetInverseView();
    return Vec3(iV.m[1][0], iV.m[1][1], iV.m[1][2]).Normalize();
}
Vec3 CameraBase::GetCameraFront() {
    using namespace DirectX;
    Matrix iV = this->GetInverseView();
    return Vec3(iV.m[2][0], iV.m[2][1], iV.m[2][2]).Normalize();
}
Vec3 CameraBase::GetCameraPos() {
    using namespace DirectX;
    Matrix iV = this->GetInverseView();
    return Vec3(iV.m[3][0], iV.m[3][1], iV.m[3][2]);
}

DirectX::XMMATRIX CameraLookAt::MakeView() {
    return DirectX::XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
}

DirectX::XMMATRIX CameraUseAngle::MakeView() {
    using namespace DirectX;
    XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    XMVECTOR Front = Transform.r[2];
    Vec3 front;
    XMStoreFloat3(&front, Front);

    eye = focus - (front * range);

    return CameraLookAt::MakeView();
}
void CameraUseAngle::Update() {
    // UpdateUseImGui();
    UpdateAllCameraMatrix();

    CCL::SetViewProjection(GetViewProjectionMatrix(), 0);
    CCL::SetCameraPosition(GetCameraPos(), 0);
}
void CameraUseAngle::UpdateUseImGui() {
    IGfilter(
    using namespace ImGui;
    Begin("Camera");
    
    InputFloat3("target", &focus.x);
    InputFloat3("Angle", &angle.x);
    InputFloat("Range", &range);


    End();
    )
}
void CameraController::Update() {
    if (InputPrimitive::State(VK_LMENU)) {
        if (InputPrimitive::State(VK_RBUTTON)) {
            Vec3 right = camera->GetCameraRight();
            Vec3 up = camera->GetCameraUp();
            Vec2 cursorDelta = -Cursor::GetCursorDelta();
            Vec3 delta = ((right * cursorDelta.x) + (up * -cursorDelta.y)) * 0.0075f;

            camera->focus += delta;
        }
        else if (InputPrimitive::State(VK_LBUTTON)) {
            Vec2 delta = Cursor::GetCursorDelta();
            camera->angle.y += delta.x * PIdiv180;
            camera->angle.x += delta.y * PIdiv180;

            camera->angle.x = clamp(camera->angle.x, -(PIdiv2 - PIdiv180), (PIdiv2 - PIdiv180));
            if (camera->angle.y < -PI) { camera->angle.y += PIx2; }
            if (camera->angle.y > +PI) { camera->angle.y -= PIx2; }
        }

        if (CCL::GetMSG().message == WM_MOUSEWHEEL) {
            velocity -= InputPrimitive::WheelDown();
            CCL::SetWindowMessage(WM_NULL);
        }
    }
    
    velocity *= 0.9f;
    if (fabsf(velocity) < 1.0f) { velocity = 0.0f; }
    camera->range += velocity * CCL::GetElapsedTime();
}