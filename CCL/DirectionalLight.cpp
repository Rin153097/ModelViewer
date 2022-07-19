#include "framework.h"
#include "CCL.h"

#include "Template/inlines.h"
#include "DirectionalLight.h"

void DirectionalLight::UpdateLightDirect() {
    using namespace DirectX;
    Matrix baseTransform = {
        1, 0, 0, 0,
        0, 0, 1, 0,
        0,-1, 0, 0,
        0, 0, 0, 1
    };
    XMMATRIX Rotation = XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    
    XMMATRIX Transform = baseTransform.GetXM() * Rotation;
    Matrix transform;
    XMStoreFloat4x4(&transform, Transform);

    direct = transform.GetFront();
}
void DirectionalLight::UpdateUseImGui() {

    IGfilter(
    using namespace ImGui;
    Begin("Light");
    SliderFloat3("Angle", &angle.x, -PI, PI);
    ImGui::InputFloat3("Direct", &direct.x);
    End();
    )
}
void DirectionalLight::Update() {
    // UpdateUseImGui();
    UpdateLightDirect();
    CCL::SetLightDirect(GetLightDirect());

    CCL::SetAmbientLight(Vec4(0.75f, 0.75f, 0.75f, 1.0f));
}