#include "OffScreenWindow.h"
#include "SpriteModule.h"
#include "misc.h"

OffScreenWindow::OffScreenWindow(ID3D11Device* device) {
    Vertex vertices[] = {
        {{-1.0f, +1.0f, 0.0f, 0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.0f,0.0f}},
        {{+1.0f, +1.0f, 0.0f, 0.0f}, {1.0f,1.0f,1.0f,1.0f}, {1.0f,0.0f}},
        {{-1.0f, -1.0f, 0.0f, 0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.0f,1.0f}},
        {{+1.0f, -1.0f, 0.0f, 0.0f}, {1.0f,1.0f,1.0f,1.0f}, {1.0f,1.0f}},
    };

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bufferDesc.StructureByteStride = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.ReleaseAndGetAddressOf());

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT },
        { "VIEW_COORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT }
    };

    // SetShader
    CreateVsFromCso(device, "./resources/Shader/windowQuadVS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(),inputElementDesc, _countof(inputElementDesc));
    CreatePsFromCso(device, "./resources/Shader/windowQuadPS.cso", pixelShader.ReleaseAndGetAddressOf());
}

void OffScreenWindow::blit(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** shaderResourceView,
    uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* opt_replacedPixelShader,
    float dx, float dy, float dw, float dh,
    float r, float g, float b, float a,
    float radian,
    float sx, float sy, float viewScaleX, float viewScaleY
) {
    D3D11_VIEWPORT viewport = {};
    UINT numViewPort = 1;
    dc->RSGetViewports(&numViewPort, &viewport);

    UINT stride = sizeof(Vertex); 
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    dc->IASetInputLayout(inputLayout.Get());

    dc->VSSetShader(vertexShader.Get(), nullptr, 0);
    opt_replacedPixelShader ? dc->PSSetShader(opt_replacedPixelShader, nullptr, 0) :
        dc->PSSetShader(pixelShader.Get(), nullptr, 0);


    const size_t indexCount = 4;

    float px[indexCount];
    float py[indexCount];

    // vertex initial position
    // left_top 
    px[0] = dx;         py[0] = dy;
    // right_top
    px[1] = dx + dw;    py[1] = dy;
    // left_bottom 
    px[2] = dx;         py[2] = dy + dh;
    // right_bottom
    px[3] = dx + dw;    py[3] = dy + dh;


    float centerX = dx + dw * 0.5f;
    float centerY = dy + dh * 0.5f;
    // pivot to center for rotate
    {
        px[0] -= centerX;
        px[1] -= centerX;
        px[2] -= centerX;
        px[3] -= centerX;
        py[0] -= centerY;
        py[1] -= centerY;
        py[2] -= centerY;
        py[3] -= centerY;
    }

    // rotation
    {
        // rotation data
        float cos = cosf(DirectX::XMConvertToRadians(radian));
        float sin = sinf(DirectX::XMConvertToRadians(radian));
        float tx, ty;
        // rotation
        for (size_t i = 0; i < indexCount; ++i) {
            tx = px[i]; ty = py[i];
            px[i] = cos * tx + -sin * ty;
            py[i] = sin * tx + cos * ty;
        }
    }

    // inverse pivot 
    {
        px[0] += centerX;
        px[1] += centerX;
        px[2] += centerX;
        px[3] += centerX;
        py[0] += centerY;
        py[1] += centerY;
        py[2] += centerY;
        py[3] += centerY;
    }

    // to NDC 
    {
        for (size_t i = 0; i < indexCount; ++i) {
            px[i] = 2.0f * px[i] / viewport.Width - 1.0f;
            py[i] = 1.0f - 2.0f * py[i] / viewport.Height;
        }
    }

    D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
    HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    Vertex* vertices = reinterpret_cast<Vertex*>(mappedSubresource.pData);
    if (vertices != nullptr) {
        for (size_t i = 0; i < indexCount; ++i) {
            vertices[i].position = { px[i],py[i], 0.0f, 0.0f };
            vertices[i].color = { r,g,b,a };

            vertices[i].texcoord.x = (sx + (i % 2) * viewScaleX);
            vertices[i].texcoord.y = (sy + (i / 2) * viewScaleY);
        }
    }

    dc->Unmap(vertexBuffer.Get(), 0);
    dc->PSSetShaderResources(startSlot, numViews, shaderResourceView);

    dc->Draw(4, 0);
}