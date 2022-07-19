#include "CCL.h"
#include "PrimitiveModel.h"

#include "Template/inlines.h"

void ArrowModel::RenderArrow(RenderSystem* renderSystem, const Matrix& world, const Vec4& color) {
    Render(renderSystem, world, color);
}
void TransformModel::RenderTransform(RenderSystem* renderSystem, const Matrix& world) {
    Render(renderSystem, world, Vec4(1.0f));
}

void GridModel::RenderGrid(RenderSystem* renderSystem, const Matrix& world) {
    using namespace DirectX;

    XMMATRIX S = XMMatrixScaling(2.0f, 1.0f, 10.0f);
    for (int i = 0; i < 11; ++i) {
        float x = -5.0f + i;
        XMMATRIX T = XMMatrixTranslation(x, 0.0f, 0.0f);
        Matrix w; XMStoreFloat4x4(&w, S * T);
        w = world * w;

        Render(renderSystem, w, { 1.0f }, nullptr);
    }
    XMMATRIX R = XMMatrixRotationY(Deg90);
    for (int i = 0; i < 11; ++i) {
        float z = -5.0f + i;
        XMMATRIX T = XMMatrixTranslation(0.0f, 0.0f, z);
        Matrix w; XMStoreFloat4x4(&w, S * R * T);
        w = world * w;

        Render(renderSystem, w, { 1.0f }, nullptr);
    }
}