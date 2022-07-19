#include "spriteBatch.h"
#include "misc.h"
#include <WICTextureLoader.h>
#include <sstream>

SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename,size_t maxSprites) 
: maxVertices(maxSprites * 6) {

    // // Vertexの初期値
    // Vertex vertices[]{
    //     {{-1.0f, +1.0f,0.0f},{1,1,1,1}, {0,0}},
    //     {{+1.0f, +1.0f,0.0f},{1,1,1,1}, {1,0}},
    //     {{-1.0f, -1.0f,0.0f},{1,1,1,1}, {0,1}},
    //     {{+1.0f, -1.0f,0.0f},{1,1,1,1}, {1,1}},
    // };
    std::unique_ptr<vertex[]> vertices = std::make_unique<vertex[]>(maxVertices);

    // 頂点バッファオブジェクトの生成
    // GPU(読み取りのみ) と CPU(書き込みのみ) によるアクセスを可能にする
    D3D11_BUFFER_DESC bufferDesc = {};                      // リソースバッファについて説明  
    bufferDesc.ByteWidth = sizeof(vertex) * static_cast<UINT>(maxVertices);  // バッファサイズ
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;                 // 読み書き方法の特定     今回は 読み　GPU　書き CPUを指定
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;        // バッファをパイプラインにバインドする方法を特定（フラグは組み合わせ可能）今回は頂点バッファとしてバインド
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPUアクセスフラグまたは、CPUアクセスが必要ない場合 0 リソースに許可されるCPUのアクセスタイプを指定 // D3D11_CPU_ACCESS_WRITE D3D11_CPU_ACCESS_READ
    bufferDesc.MiscFlags = 0;                               // その他のフラグ
    bufferDesc.StructureByteStride = 0;                     // ???

    D3D11_SUBRESOURCE_DATA subresourceData{};               // サブリソースを初期化するためのデータを指定
    subresourceData.pSysMem = vertices.get();               // 初期化データへのポインタ
    subresourceData.SysMemPitch = 0;                        // 次までのサイズ　？？？
    subresourceData.SysMemSlicePitch = 0;                   // 次の深さまでの距離 ???

    HRESULT hr = S_OK;
    hr = device->CreateBuffer(                              // バッファーを作成
        &bufferDesc,                                        // バッファーを記述する構造体へのポインター
        &subresourceData,                                   // 初期化データを記述する構造体へのポインター
        vertexBuffer.GetAddressOf()                        // 作成されたバッファーオブジェクトへのポインターのアドレス
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));              // CreateBufferはバッファー作成に十分なメモリがないならE_OUTOFMEMORYを返す

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

    CreateVsFromCso(device, "./resources/Shader/spriteVS.cso", vertexShader.GetAddressOf(),
        inputLayout.GetAddressOf(), inputElementDesc, 3);

    CreatePsFromCso(device, "./resources/Shader/spritePS.cso", pixelShader.GetAddressOf());


    //画像ファイルのロードとシェーダーリソースビューオブジェクトの生成
    ID3D11Resource* resource = {}; // すべてのリソースに共通のアクションを提供
    hr = DirectX::CreateWICTextureFromFile(
        device,                 //
        filename,               // 
        &resource,              // 
        shaderResourceView.GetAddressOf()   // 
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    resource->Release();

    //テクスチャ情報の取得
    ID3D11Texture2D* texture2d = {};
    hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    texture2d->GetDesc(&texture2dDesc);
    texture2d->Release();
}

// 描画
void SpriteBatch::render(ID3D11DeviceContext* immediateContext,
    float dx, float dy, float dw, float dh,
    float r, float g, float b, float a, float deg_angle) {
    render(immediateContext,
        dx, dy, dw, dh,
        r, g, b, a, deg_angle, 0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height));
}

void SpriteBatch::render(ID3D11DeviceContext* immediateContext,
    float dx, float dy, float dw, float dh,
    float r, float g, float b, float a,
    float deg_angle,
    float sx, float sy, float sw, float sh)
{
    D3D11_VIEWPORT viewport = {};
    UINT num_viewports = 1;
    immediateContext->RSGetViewports(&num_viewports, &viewport);

    // Set each sprite's vertices coordinate to screen space
    //
    //  (x0, y0) *----* (x1, y1) 
    //	         |   /|
    //	         |  / |
    //	         | /  |
    //	         |/   |
    //  (x2, y2) *----* (x3, y3) 

    // left-top
    float x0 = dx;
    float y0 = dy;
    // right-top
    float x1 = dx + dw;
    float y1 = dy;
    // left-bottom
    float x2 = dx;
    float y2 = dy + dh;
    // right-bottom
    float x3 = dx + dw;
    float y3 = dy + dh;

#if 0
#if 0
    std::function<void(float&, float&, float, float, float, float)> rotate{ [](float& x, float& y, float cx, float cy, float sin, float cos)
    {
        x -= cx;
        y -= cy;

        float tx{ x }, ty{ y };
        x = cos * tx + -sin * ty;
        y = sin * tx + cos * ty;

        x += cx;
        y += cy;
    } };
#endif
    // UNIT.09
    float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
    float sin{ sinf(DirectX::XMConvertToRadians(angle)) };

    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    rotate(x0, y0, cx, cy, sin, cos);
    rotate(x1, y1, cx, cy, sin, cos);
    rotate(x2, y2, cx, cy, sin, cos);
    rotate(x3, y3, cx, cy, sin, cos);
#else
    // Translate sprite's centre to origin (rotate centre)
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    x0 -= cx;
    y0 -= cy;
    x1 -= cx;
    y1 -= cy;
    x2 -= cx;
    y2 -= cy;
    x3 -= cx;
    y3 -= cy;

    // Rotate each sprite's vertices by angle
    float tx, ty;
    float cos = cosf(DirectX::XMConvertToRadians(deg_angle));
    float sin = sinf(DirectX::XMConvertToRadians(deg_angle));
    tx = x0;
    ty = y0;
    x0 = cos * tx + -sin * ty;
    y0 = sin * tx + cos * ty;
    tx = x1;
    ty = y1;
    x1 = cos * tx + -sin * ty;
    y1 = sin * tx + cos * ty;
    tx = x2;
    ty = y2;
    x2 = cos * tx + -sin * ty;
    y2 = sin * tx + cos * ty;
    tx = x3;
    ty = y3;
    x3 = cos * tx + -sin * ty;
    y3 = sin * tx + cos * ty;

    // Translate sprite's centre to original position
    x0 += cx;
    y0 += cy;
    x1 += cx;
    y1 += cy;
    x2 += cx;
    y2 += cy;
    x3 += cx;
    y3 += cy;
#endif
    // convet to NDC
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    float u0 = sx / texture2dDesc.Width;
    float v0 = sy / texture2dDesc.Height;
    float u1 = (sx + sw) / texture2dDesc.Width;
    float v1 = (sy + sh) / texture2dDesc.Height;
    vertices.push_back({ { x0, y0 , 0 }, { r, g, b, a }, { u0, v0 } });
    vertices.push_back({ { x1, y1 , 0 }, { r, g, b, a }, { u1, v0 } });
    vertices.push_back({ { x2, y2 , 0 }, { r, g, b, a }, { u0, v1 } });
    vertices.push_back({ { x2, y2 , 0 }, { r, g, b, a }, { u0, v1 } });
    vertices.push_back({ { x1, y1 , 0 }, { r, g, b, a }, { u1, v0 } });
    vertices.push_back({ { x3, y3 , 0 }, { r, g, b, a }, { u1, v1 } });
}
void SpriteBatch::Begin(ID3D11DeviceContext* immediateContext) {
    vertices.clear();

    immediateContext->VSSetShader(                                 // デバイスに頂点シェーダを設定
        vertexShader.Get(),                                              // 頂点シェーダへのポインタ 
        nullptr,                                                    // 
        0                                                           // 配列内のクラス インスタンス インターフェイスの数
    );
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
}
void SpriteBatch::End(ID3D11DeviceContext* immediateContext) {
    HRESULT hr = S_OK;
    D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
    hr = immediateContext->Map(    // サブリソースに含まれるデータへのポインタを取得し、そのサブリソースへのGPUアクセスを拒否
        vertexBuffer.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedSubresource
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    size_t vertexCount = vertices.size();
    _ASSERT_EXPR(maxVertices >= vertexCount, "Buffer overflow");
    vertex* data = reinterpret_cast<vertex*>(mappedSubresource.pData);
    if (data != nullptr) {
        const vertex* p = vertices.data();
        const size_t dataSize = maxVertices * sizeof(vertex);
        const size_t pSize = vertexCount * sizeof(vertex);
        memcpy_s(data, dataSize, p, pSize);
    }

    immediateContext->Unmap(vertexBuffer.Get(), 0);

    UINT stride = sizeof(vertex);
    UINT offset = 0;
    immediateContext->IASetVertexBuffers(
        0,                  // バインドする最初の入力スロット
        1,                  // 配列内の頂点バッファーの数
        vertexBuffer.GetAddressOf(),     // 頂点バッファーの配列へのポインタ
        &stride,            // ストライド値の配列へのポインター
        &offset             // オフセットの配列へのポインタ
    );

    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //プリミティブ型に関する情報、入力アセンブラ・ステージ入力データを記述するデータ順序をバインド

    immediateContext->IASetInputLayout(inputLayout.Get());              // 入力レイアウト・オブジェクトを入力アセンブラ・ステージにバインド


    immediateContext->Draw(static_cast<UINT>(vertexCount), 0);                                  // 頂点数 最初の頂点のインデックス
}

SpriteBatch::~SpriteBatch() {}