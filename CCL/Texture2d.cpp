#include "Texture2d.h"

void Texture2dManager::Begin(int index) {
    renderSystem->SwitchBlendState(blendType);
    renderSystem->SwitchDepthStencil(depthState);
    renderSystem->SwitchRasterizer(rasterizerState);

    this->textures[index]->Begin(renderSystem->immediateContext.Get());
}
void Texture2dManager::Render(int index,
    float posX, float posY, float drawW, float drawH,
    int drawPositionIndex,
    float r, float g, float b, float a,
    float angle
) {
    Texture2d::DrawData drawData(Vec2(posX, posY), Vec2(drawW, drawH), drawPositionIndex, angle, Vec4(r, g, b, a));
    this->textures[index]->Render(renderSystem->immediateContext.Get(), drawData);
}
void Texture2dManager::Render(int index, Texture2d::DrawData data) {
    this->textures[index]->Render(renderSystem->immediateContext.Get(),data);
}
void Texture2dManager::End(int index) {
    this->textures[index]->End(renderSystem->immediateContext.Get());
}

int Texture2dManager::LoadSprite(const wchar_t* filename, int max, Vec2 drawSize, size_t textureXnum = 1) {
    textures.emplace_back(Texture2d::LoadTexture(renderSystem->device.Get(), filename, max, drawSize, textureXnum));
    
    return static_cast<int>(textures.size() - 1);
}