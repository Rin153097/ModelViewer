#pragma once

#include <d3d11.h>
#include "Vector.h"

#include <wrl.h>

#include <vector>

#include "SpriteModule.h"

class SpriteBatch {
public:
    struct vertex {
        Vec3 position;
        Vec4 color;
        Vec2 texcoord;
    };
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader;  // 頂点シェーダーステージを制御する実行可能プログラムを管理
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader;    // 同様にこちらはピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout;    // 頂点データをアセンブラーステージに送り込む方法を定義
    Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer;        // 非構造化メモリであるバッファー リソースにアクセス バッファは通常、頂点データまたはインデックス データを格納

    // about sprite
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView; // レンダリング中にシェーダがアクセスできるサブリソースを指定 (シェーダーリソース　定数バッファ・テクスチャバッファ、テクスチャ)
    D3D11_TEXTURE2D_DESC texture2dDesc; // 2Dテクスチャについて説明

    const size_t maxVertices;
    std::vector<vertex> vertices;

public:
    void render(ID3D11DeviceContext* immediateContext,
        float dx, float dy, float dw, float dh,
        float r, float g, float b, float a, float angle);

    void render(ID3D11DeviceContext* immediateContext,
        float dx, float dy, float dw, float dh,
        float r, float g, float b, float a,
        float angle,
        float sx, float sy, float sw, float sh);

    SpriteBatch(ID3D11Device* device, const wchar_t* filename,size_t maxSprites);
    ~SpriteBatch();

    void Begin(ID3D11DeviceContext* immediateContext);
    void End(ID3D11DeviceContext* immediateContext);
};