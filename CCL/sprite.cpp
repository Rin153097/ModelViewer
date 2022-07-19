#include "sprite.h"
#include "SpriteModule.h"
#include <sstream>

sprite::sprite(ID3D11Device* device,const wchar_t* filename) {
    // Vertex�̏����l
    vertex vertices[]{
        {{-1.0f, +1.0f,0.0f},{1,1,1,1}, {0,0}},
        {{+1.0f, +1.0f,0.0f},{1,1,1,1}, {1,0}},
        {{-1.0f, -1.0f,0.0f},{1,1,1,1}, {0,1}},
        {{+1.0f, -1.0f,0.0f},{1,1,1,1}, {1,1}},
    };

    // ���_�o�b�t�@�I�u�W�F�N�g�̐���
    // GPU(�ǂݎ��̂�) �� CPU(�������݂̂�) �ɂ��A�N�Z�X���\�ɂ���
    D3D11_BUFFER_DESC bufferDesc;                        // ���\�[�X�o�b�t�@�ɂ��Đ���  
    bufferDesc.ByteWidth = sizeof(vertices);               // �o�b�t�@�T�C�Y
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;                // �ǂݏ������@�̓���     ����� �ǂ݁@GPU�@���� CPU���w��
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // �o�b�t�@���p�C�v���C���Ƀo�C���h������@�����i�t���O�͑g�ݍ��킹�\�j����͒��_�o�b�t�@�Ƃ��ăo�C���h
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU�A�N�Z�X�t���O�܂��́ACPU�A�N�Z�X���K�v�Ȃ��ꍇ 0 ���\�[�X�ɋ������CPU�̃A�N�Z�X�^�C�v���w�� // D3D11_CPU_ACCESS_WRITE D3D11_CPU_ACCESS_READ
    bufferDesc.MiscFlags = 0;                              // ���̑��̃t���O
    bufferDesc.StructureByteStride = 0;                    // ???

    D3D11_SUBRESOURCE_DATA subresourceData{};              // �T�u���\�[�X�����������邽�߂̃f�[�^���w��
    subresourceData.pSysMem = vertices;                    // �������f�[�^�ւ̃|�C���^
    subresourceData.SysMemPitch = 0;                       // ���܂ł̃T�C�Y�@�H�H�H
    subresourceData.SysMemSlicePitch = 0;                  // ���̐[���܂ł̋��� ???

    HRESULT hr = { S_OK };
    hr = device->CreateBuffer(                              // �o�b�t�@�[���쐬
        &bufferDesc,                                        // �o�b�t�@�[���L�q����\���̂ւ̃|�C���^�[
        &subresourceData,                                   // �������f�[�^���L�q����\���̂ւ̃|�C���^�[
        vertexBuffer.ReleaseAndGetAddressOf()               // �쐬���ꂽ�o�b�t�@�[�I�u�W�F�N�g�ւ̃|�C���^�[�̃A�h���X
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));  // CreateBuffer�̓o�b�t�@�[�쐬�ɏ\���ȃ��������Ȃ��Ȃ�E_OUTOFMEMORY��Ԃ�

    // ���̓��C�A�E�g�v�f�̒ǉ�
    //���̓��C�A�E�g�I�u�W�F�N�g
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] // ���̓A�Z���u���[�X�e�[�W�̒P��v�f�̐���
    {
        {
        "POSITION",                     // �V�F�[�_�[���̓V�O�l�`���̂��̗v�f�Ɋ֘A�t����ꂽ HLSL �Z�}���e�B�b�N
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

// �`��
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
        0,                  // �o�C���h����ŏ��̓��̓X���b�g
        1,                  // �z����̒��_�o�b�t�@�[�̐�
        vertexBuffer.GetAddressOf(),     // ���_�o�b�t�@�[�̔z��ւ̃|�C���^
        &stride,            // �X�g���C�h�l�̔z��ւ̃|�C���^�[
        &offset             // �I�t�Z�b�g�̔z��ւ̃|�C���^
    );

    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); //�v���~�e�B�u�^�Ɋւ�����A���̓A�Z���u���E�X�e�[�W���̓f�[�^���L�q����f�[�^�������o�C���h

    immediateContext->IASetInputLayout(inputLayout.Get());              // ���̓��C�A�E�g�E�I�u�W�F�N�g����̓A�Z���u���E�X�e�[�W�Ƀo�C���h

    immediateContext->VSSetShader(                                 // �f�o�C�X�ɒ��_�V�F�[�_��ݒ�
        vertexShader.Get(),                                              // ���_�V�F�[�_�ւ̃|�C���^ 
        nullptr,                                                    // 
        0                                                           // �z����̃N���X �C���X�^���X �C���^�[�t�F�C�X�̐�
    );
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    

    const int verNum = 4;

    float px[verNum];
    float py[verNum];
    // �����l�����
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

    //��]
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

    // �ϊ�
    for (int i = 0; i < verNum; ++i) {
        px[i] = 2.0f * px[i] / viewport.Width - 1.0f;
        py[i] = 1.0f - 2.0f * py[i] / viewport.Height;
    }

    HRESULT hr = S_OK;
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediateContext->Map(    // �T�u���\�[�X�Ɋ܂܂��f�[�^�ւ̃|�C���^���擾���A���̃T�u���\�[�X�ւ�GPU�A�N�Z�X������
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

    immediateContext->Draw(4, 0);                                  // ���_�� �ŏ��̒��_�̃C���f�b�N�X
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
