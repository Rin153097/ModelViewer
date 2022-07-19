#pragma once

#include "Vector.h"
#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

class OffScreenWindow {
    struct Vertex {
        Vec4 position;
        Vec4 color;
        Vec2 texcoord;
    };

public:
    OffScreenWindow(ID3D11Device* device);
    virtual ~OffScreenWindow() = default;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer;

public:
    void blit(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** shaderResourceView,
        uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* opt_replacedPixelShader,
        float dx, float dy, float dw, float dh,
        float r, float g, float b, float a,
        float radian,
        float sx, float sy, float sw, float sh
        );
};