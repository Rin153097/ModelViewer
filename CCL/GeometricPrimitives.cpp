#include "GeometricPrimitives.h"
#include "GeometricPrimitive.h"
#include "SpriteModule.h"

#include "misc.h"

GeometricPrimitive::GeometricPrimitive(ID3D11Device* device) {

    // コントロール数 8 * 各頂点の法線の数 3
    const int verticesNum = 24;
    // 6面で一つの面で３角形は二つ必要（四角形のため） 6 * 2 = 12
    // ３角形なので 12 * 3 個の頂点情報が必要
    const int indicesNum = 36;

    const float halfSize = 0.5f;

    enum SIDE {
        right = 0,
        left,
        top,
        bottom,
        front,
        back,

        num
    };

    const float pos[SIDE::num] = {
        +halfSize,
        -halfSize,
        +halfSize,
        -halfSize,
        +halfSize,
        -halfSize,
    };
    
    vertex vertices[verticesNum];
    int index = 0;
    for (int x = 0; x < 2; ++x) {
        for (int y = 0; y < 2; ++y) {
            for (int z = 0; z < 2; ++z) {
                vertices[index].position    = { pos[right + x],pos[top + y],pos[front + z] };
                vertices[index++].normal    = { vertices[index].position.x * 2.0f,0.0f,0.0f };
                vertices[index].position    = { pos[right + x],pos[top + y],pos[front + z] };
                vertices[index++].normal    = { 0.0f,vertices[index].position.y * 2.0f,0.0f };
                vertices[index].position    = { pos[right + x],pos[top + y],pos[front + z] };
                vertices[index++].normal    = { 0.0f,0.0f,vertices[index].position.z * 2.0f };
            }
        }
    }
    uint32_t indeces[indicesNum] = {
        0,9,3,
        0,6,9,
        12,15,18,
        15,21,18,
        1,4,16,
        1,16,13,
        7,22,10,
        7,19,22,
        2,14,8,
        14,20,8,
        5,11,23,
        5,23,17,
    };

    CreateComBuffers(device, vertices, verticesNum, indeces, indicesNum);

    HRESULT hr = S_OK;

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    CreateVsFromCso(device, "./resources/Shader/geometricPrimitiveVS.cso", vertexShader.GetAddressOf(),
        inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    CreatePsFromCso(device, "./resources/Shader/geometricPrimitivePS.cso", pixelShader.GetAddressOf());

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(constants);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void GeometricPrimitive::CreateComBuffers(ID3D11Device* device, vertex* vertices, size_t vertexCount,
    uint32_t* indices, size_t indexCount) {
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertexCount);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indexCount);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    subresourceData.pSysMem = indices;

    hr = device->CreateBuffer(&bufferDesc, &subresourceData, indexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void GeometricPrimitive::render(ID3D11DeviceContext* immediateContext,
    const Matrix& world, const Vec4& materialColor) {
    uint32_t stride = sizeof(vertex);
    uint32_t offset = 0;

    immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediateContext->IASetInputLayout(inputLayout.Get());

    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    constants data = { world,materialColor };
    immediateContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
    immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    D3D11_BUFFER_DESC bufferDesc = {};
    indexBuffer->GetDesc(&bufferDesc);
    immediateContext->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
}