#pragma once
#include "SkinnedMesh.h"
#include "RenderSystem.h"

class ArrowModel : public SkinMesh {
public:
    ArrowModel(ID3D11Device* device)
        : SkinMesh(nullptr, device, "./resources/CCL/arrow.fbx", 0.0f, true, COORDINATE_TYPE::RIGHT_YUP,
            RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, Vec4(1.0f), false) {}
    void RenderArrow(RenderSystem* renderSystem, const Matrix& world = Matrix(), const Vec4& color = Vec4());
};

class TransformModel : public SkinMesh {
public:
    TransformModel(ID3D11Device* device)
        : SkinMesh(nullptr, device, "./resources/CCL/transform.fbx", 0.0f, true, COORDINATE_TYPE::RIGHT_ZUP,
            RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, Vec4(1.0f), false) {}
    void RenderTransform(RenderSystem* renderSystem, const Matrix& world = Matrix());
};

class GridModel : public SkinMesh {
public:
    GridModel(ID3D11Device* device)
        : SkinMesh(nullptr, device, "./resources/CCL/line.fbx", 0.0f, true, COORDINATE_TYPE::RIGHT_YUP,
            RASTERIZER_STATE::FILL_NOT_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, Vec4(1.0f), false) {}

    void RenderGrid(RenderSystem* renderSystem, const Matrix& world);
};


class PrimitiveModel {
    std::unique_ptr<ArrowModel> arrow;
    std::unique_ptr<TransformModel> axis;
    std::unique_ptr<GridModel> grid;

    RenderSystem* renderSystem;

public:
    PrimitiveModel(RenderSystem* renderSystem) : renderSystem(renderSystem) {
        axis = std::make_unique<TransformModel>(renderSystem->device.Get());
        grid = std::make_unique<GridModel>(renderSystem->device.Get());
    }

public:
    void RenderArrow(const Matrix& world = Matrix(), const Vec4& color = Vec4()) 
    { arrow->RenderArrow(renderSystem, world, color); }
    void RenderTransform(const Matrix& world = Matrix())
    { axis->RenderTransform(renderSystem, world); }
    void RenderGrid(const Matrix& world = Matrix())
    { grid->RenderGrid(renderSystem, world); }
};