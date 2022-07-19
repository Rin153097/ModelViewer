#pragma once

#include <memory>
#include "spriteBatch.h"
#include "RenderSystem.h"

class Texture2d {
public:
    struct DrawData {
        Vec4 color;

        Vec2 position;
        Vec2 drawSize;
        int  drawPositionIndex;

        float angle;

        DrawData(Vec2 _position = Vec2(), Vec2 _drawSize = Vec2(), int _drawPositionIndex = 0, float _angle = 0.0f, Vec4 _color = Vec4(1.0f)) :
            position(_position), drawSize(_drawSize), drawPositionIndex(_drawPositionIndex), angle(_angle), color(_color) {}
    };
private:
    Texture2d(ID3D11Device* device, const wchar_t* filename, size_t maxDrawNum, Vec2 drawSize, size_t textureXnum = 1) :
        drawSize(drawSize), textureXnum(textureXnum) {
        tex = std::make_unique<SpriteBatch>(device, filename, maxDrawNum);
    }
public:
    static Texture2d* LoadTexture(ID3D11Device* device, const wchar_t* filename, size_t maxDrawNum, Vec2 drawSize, size_t textureXnum = 1) {
        return new Texture2d(device, filename, maxDrawNum, drawSize, textureXnum);
    }

public:
    std::unique_ptr<SpriteBatch> tex;

    Vec2 drawSize;
    size_t textureXnum;

public:
    void Begin(ID3D11DeviceContext* dc) { tex->Begin(dc); }
    void Render(ID3D11DeviceContext* dc, const DrawData& drawData) {
        tex->render(dc,
            drawData.position.x, drawData.position.y,
            drawData.drawSize.x, drawData.drawSize.y,
            drawData.color.x, drawData.color.y, drawData.color.z, drawData.color.w,
            drawData.angle,
            drawSize.x * (drawData.drawPositionIndex % textureXnum), drawSize.y * (drawData.drawPositionIndex / textureXnum),
            drawSize.x, drawSize.y);
    }
    void End(ID3D11DeviceContext* dc) { tex->End(dc); }
};

class Texture2dManager {
private:
    RenderSystem*       renderSystem;

    BLEND_TYPE          blendType;
    DEPTH_STATE         depthState;
    RASTERIZER_STATE    rasterizerState;

    std::vector<Texture2d*> textures;
public:
    Texture2dManager() : Texture2dManager(nullptr) {}
    Texture2dManager(RenderSystem* renderSystem) : 
        renderSystem(renderSystem), 
        blendType(BLEND_TYPE::ALPHA), depthState(DEPTH_STATE::DISABLE_NOT_WRITE), rasterizerState(RASTERIZER_STATE::FILL_NOT_CULL) {}
    ~Texture2dManager() {
        for (Texture2d* tex : textures) {
            delete tex;
        }
    }
public:
    void Begin(int index);
    void Render(int index,
        float posX, float posY, float drawW, float drawH,
        int drawPositionIndex = 0,
        float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
        float angle = 0.0f
    );
    void Render(int index, Texture2d::DrawData data);
    void End(int index);

    int LoadSprite(const wchar_t* filename, int max, Vec2 drawSize, size_t textureXnum);

    void SetRenderSystem(RenderSystem* renderSystem) { this->renderSystem = renderSystem; }
};