#pragma once

class PrimitiveRect {
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer;

public:
    PrimitiveRect(RenderSystem* renderSystem);

public:
    void Render(RenderSystem* renderSystem,
        float dx, float dy,
        float dw, float dh,
        float r, float g, float b, float a, float radian);

    struct Vertex {
        Vec3 position;
        Vec4 color;
    };
};