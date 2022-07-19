#include "sprite.h"
#include "SpriteModule.h"
#include <sstream>

sprite::sprite(ID3D11Device* device,const wchar_t* filename) {
    // Vertexの初期値
    vertex vertices[]{
        {{-1.0f, +1.0f,0.0f},{1,1,1,1}, {0,0}},
        {{+1.0f, +1.0f,0.0f},{1,1,1,1}, {1,0}},
        {{-1.0f, -1.0f,0.0f},{1,1,1,1}, {0,1}},
        {{+1.0f, -1.0f,0.0f},{1,1,1,1}, {1,1}},
    };

    // 頂点バッファオブジェクトの生成
    // GPU(読み取りのみ) と CPU(書き込みのみ) によるアクセスを可能にする
    D3D11_BUFFER_DESC bufferDesc;                        // リソースバッファについて説明  
    bufferDesc.ByteWidth = sizeof(vertices);               // バッファサイズ
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;                // 読み書き方法の特定     今回は 読み　GPU　書き CPUを指定
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // バッファをパイプラインにバインドする方法を特定（フラグは組み合わせ可能）今回は頂点バッファとしてバインド
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPUアクセスフラグまたは、CPUアクセスが必要ない場合 0 リソースに許可されるCPUのアクセスタイプを指定 // D3D11_CPU_ACCESS_WRITE D3D11_CPU_ACCESS_READ
    bufferDesc.MiscFlags = 0;                              // その他のフラグ
    bufferDesc.StructureByteStride = 0;                    // ???

    D3D11_SUBRESOURCE_DATA subresourceData{};              // サブリソースを初期化するためのデータを指定
    subresourceData.pSysMem = vertices;                    // 初期化データへのポインタ
    subresourceData.SysMemPitch = 0;                       // 次までのサイズ　？？？
    subresourceData.SysMemSlicePitch = 0;                  // 次の深さまでの距離 ???

    HRESULT hr = { S_OK };
    hr = device->CreateBuffer(                              // バッファーを作成
        &bufferDesc,                                        // バッファーを記述する構造体へのポインター
        &subresourceData,                                   // 初期化データを記述する構造体へのポインター
        vertexBuffer.ReleaseAndGetAddressOf()               // 作成されたバッファーオブジェクトへのポインターのアドレス
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));  // CreateBufferはバッファー作成に十分なメモリがないならE_OUTOFMEMORYを返す

    // 入力レイアウト要素の追加
    //入力レイアウトオブジェクト
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] // 入力アセンブラーステージの単一要素の説明
    {
        {
        "POSITION",                     // シェーダー入力シグネチャのこの要素に関連付けられた HLSL セマンティック
        0,                              // ???
        DXGI_FORMAT_R32G32B32_FLOAT,
        0,
        D3D11_APPEND_ALIGNED_ELEMENT,
        D3D11_INPUT_PER_VERTEX_DATA,
        0
        },
        { "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
        D3D10_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    CreateVsFromCso(device, "./resources/Shader/spriteVS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.GetAddressOf(),
        inputElementDesc, _countof(inputElementDesc));
    CreatePsFromCso(device, "./resources/Shader/spritePS.cso", pixelShader.GetAddressOf());

    LoadTextureFromFile(device, filename, shaderResourceView.GetAddressOf(), &texture2dDesc);
}

// 描画
void sprite::render(ID3D11DeviceContext* immediateContext,
    float dx, float dy, float dw, float dh,
    float r, float g, float b, float a,float deg_angle) {
    render(immediateContext,
        dx, dy, dw, dh,
        r, g, b, a, deg_angle, 0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height));
}

void sprite::render(ID3D11DeviceContext* immediateContext,
    float dx, float dy, float dw, float dh,
    float r, float g, float b, float a,
    float deg_angle,
    float sx, float sy, float sw, float sh) 
{
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediateContext->RSGetViewports(
        &num_viewports,
        &viewport
    );

    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediateContext->IASetVertexBuffers(
        0,                  // バインドする最初の入力スロット
        1,                  // 配列内の頂点バッファーの数
        vertexBuffer.GetAddressOf(),     // 頂点バッファーの配列へのポインタ
        &stride,            // ストライド値の配列へのポインター
        &offset             // オフセットの配列へのポインタ
    );

    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); //プリミティブ型に関する情報、入力アセンブラ・ステージ入力データを記述するデータ順序をバインド

    immediateContext->IASetInputLayout(inputLayout.Get());              // 入力レイアウト・オブジェクトを入力アセンブラ・ステージにバインド

    immediateContext->VSSetShader(                                 // デバイスに頂点シェーダを設定
        vertexShader.Get(),                                              // 頂点シェーダへのポインタ 
        nullptr,                                                    // 
        0                                                           // 配列内のクラス インスタンス インターフェイスの数
    );
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    

    const int verNum = 4;

    float px[verNum];
    float py[verNum];
    // 初期値を入力
    // left_top
    px[0] = dx;
    py[0] = dy;
    // right_top
    px[1] = dx + dw;
    py[1] = dy;
    // left_bottom
    px[2] = dx;
    py[2] = dy + dh;
    // right_bottom
    px[3] = dx + dw;
    py[3] = dy + dh;

    //回転
    auto Rotate = [](float& x, float& y, float cx, float cy, float angle) {
        x -= cx;
        y -= cy;

        float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
        float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
        float tx{ x }, ty{ y };
        x = (cos * tx) + (-sin * ty);
        y = (sin * tx) + (+cos * ty);

        x += cx;
        y += cy;
    };

    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    for (int i = 0; i < verNum; ++i) {
        Rotate(px[i], py[i], cx, cy, deg_angle);
    }

    // 変換
    for (int i = 0; i < verNum; ++i) {
        px[i] = 2.0f * px[i] / viewport.Width - 1.0f;
        py[i] = 1.0f - 2.0f * py[i] / viewport.Height;
    }

    HRESULT hr = S_OK;
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediateContext->Map(    // サブリソースに含まれるデータへのポインタを取得し、そのサブリソースへのGPUアクセスを拒否
        vertexBuffer.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped_subresource
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr) {
        for (int i = 0; i < 4; ++i) {
            vertices[i].position = { px[i],py[i],0 };
            vertices[i].color = { r,g,b,a };

            vertices[i].texcoord.x = (sx + (i % 2) * sw) / texture2dDesc.Width;
            vertices[i].texcoord.y = (sy + (i / 2) * sh) / texture2dDesc.Height;
        }
    }
    
    immediateContext->Unmap(vertexBuffer.Get(), 0);

    immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

    immediateContext->Draw(4, 0);                                  // 頂点数 最初の頂点のインデックス
}

sprite::~sprite() {
    shaderResourceView.Reset();
    pixelShader.Reset(); 
    inputLayout.Reset(); 
    vertexShader.Reset();
    vertexBuffer.Reset();
}

HRESULT MakeDummyTexture(
    ID3D11Device* device, ID3D11ShaderResourceView** shaderResourceView, DWORD value, UINT dimension) {
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    texture2dDesc.Width = dimension;
    texture2dDesc.Height = dimension;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    size_t texels = dimension * dimension;
    std::unique_ptr<DWORD[]>sysmem = std::make_unique<DWORD[]>(texels);
    for (size_t i = 0; i < texels; ++i) {
        sysmem[i] = value;
    }

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = sysmem.get();
    subresourceData.SysMemPitch = sizeof(DWORD) * dimension;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
    shaderResourceViewDesc.Format = texture2dDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(
        texture2d.Get(),
        &shaderResourceViewDesc,
        shaderResourceView);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}
