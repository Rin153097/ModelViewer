#pragma once

#include "Vector.h"
#include <d3d11.h>
#include <wrl.h>

class GeometricPrimitive {
public:
    struct vertex {
        Vec3 position;
        Vec3 normal;
    };
    struct constants {
        Matrix world;
        Vec4   materialColor;
    };

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        constantBuffer;

public:
    GeometricPrimitive(ID3D11Device* device);
    virtual ~GeometricPrimitive() = default;

    void render(ID3D11DeviceContext* immediateContext,
        const Matrix& world, const Vec4& materialColor);

protected:
    void CreateComBuffers(ID3D11Device* device, vertex* vertices, size_t vertexCount,
        uint32_t* indices, size_t indexCount);
};