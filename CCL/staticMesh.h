#pragma once

#include "Vector.h"
#include <d3d11.h>
#include <wrl.h>

#include <vector>
#include <fstream>

class StaticMesh {
public:
    struct Vertex {
        Vec3 position;
        Vec3 normal;
        Vec2 texcoord;
    };
    struct constants {
        Matrix world;
        Vec4   materialColor;
    };
    struct Subset {
        std::wstring usemtl;
        uint32_t indexStart = {};
        uint32_t indexCount = {};
    };
    struct Material {
        std::wstring name;
        Vec4 Ka = { 0.2f, 0.2f, 0.2f, 1.0f };
        Vec4 Kd = { 0.8f, 0.8f, 0.8f, 1.0f };
        Vec4 Ks = { 1.0f, 1.0f, 1.0f, 1.0f };

        std::wstring textureFileName[2];
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView[2];
    };
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        constantBuffer;

private:
    std::vector<Subset> subsets;
    std::vector<Material> materials;

public:
    StaticMesh(ID3D11Device* device,const wchar_t* fileName,bool flipCoordV);
    virtual ~StaticMesh() = default;

    void render(ID3D11DeviceContext* immediateContext,
        const Matrix& world, const Vec4& materialColor);

protected:
    void CreateComBuffers(ID3D11Device* device, Vertex* vertices, size_t vertexCount,
        uint32_t* indices, size_t indexCount);
};