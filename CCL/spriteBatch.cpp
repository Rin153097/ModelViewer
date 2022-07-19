#include "spriteBatch.h"
#include "misc.h"
#include <WICTextureLoader.h>
#include <sstream>

SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename,size_t maxSprites) 
: maxVertices(maxSprites * 6) {

    // // Vertex�̏����l
    // Vertex vertices[]{
    //     {{-1.0f, +1.0f,0.0f},{1,1,1,1}, {0,0}},
    //     {{+1.0f, +1.0f,0.0f},{1,1,1,1}, {1,0}},
    //     {{-1.0f, -1.0f,0.0f},{1,1,1,1}, {0,1}},
    //     {{+1.0f, -1.0f,0.0f},{1,1,1,1}, {1,1}},
    // };
    std::unique_ptr<vertex[]> vertices = std::make_unique<vertex[]>(maxVertices);

    // ���_�o�b�t�@�I�u�W�F�N�g�̐���
    // GPU(�ǂݎ��̂�) �� CPU(�������݂̂�) �ɂ��A�N�Z�X���\�ɂ���
    D3D11_BUFFER_DESC bufferDesc = {};                      // ���\�[�X�o�b�t�@�ɂ��Đ���  
    bufferDesc.ByteWidth = sizeof(vertex) * static_cast<UINT>(maxVertices);  // �o�b�t�@�T�C�Y
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;                 // �ǂݏ������@�̓���     ����� �ǂ݁@GPU�@���� CPU���w��
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;        // �o�b�t�@���p�C�v���C���Ƀo�C���h������@�����i�t���O�͑g�ݍ��킹�\�j����͒��_�o�b�t�@�Ƃ��ăo�C���h
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPU�A�N�Z�X�t���O�܂��́ACPU�A�N�Z�X���K�v�Ȃ��ꍇ 0 ���\�[�X�ɋ������CPU�̃A�N�Z�X�^�C�v���w�� // D3D11_CPU_ACCESS_WRITE D3D11_CPU_ACCESS_READ
    bufferDesc.MiscFlags = 0;                               // ���̑��̃t���O
    bufferDesc.StructureByteStride = 0;                     // ???

    D3D11_SUBRESOURCE_DATA subresourceData{};               // �T�u���\�[�X�����������邽�߂̃f�[�^���w��
    subresourceData.pSysMem = vertices.get();               // �������f�[�^�ւ̃|�C���^
    subresourceData.SysMemPitch = 0;                        // ���܂ł̃T�C�Y�@�H�H�H
    subresourceData.SysMemSlicePitch = 0;                   // ���̐[���܂ł̋��� ???

    HRESULT hr = S_OK;
    hr = device->CreateBuffer(                              // �o�b�t�@�[���쐬
        &bufferDesc,                                        // �o�b�t�@�[���L�q����\���̂ւ̃|�C���^�[
        &subresourceData,                                   // �������f�[�^���L�q����\���̂ւ̃|�C���^�[
        vertexBuffer.GetAddressOf()                        // �쐬���ꂽ�o�b�t�@�[�I�u�W�F�N�g�ւ̃|�C���^�[�̃A�h���X
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));              // CreateBuffer�̓o�b�t�@�[�쐬�ɏ\���ȃ��������Ȃ��Ȃ�E_OUTOFMEMORY��Ԃ�

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

    CreateVsFromCso(device, "./resources/Shader/spriteVS.cso", vertexShader.GetAddressOf(),
        inputLayout.GetAddressOf(), inputElementDesc, 3);

    CreatePsFromCso(device, "./resources/Shader/spritePS.cso", pixelShader.GetAddressOf());


    //�摜�t�@�C���̃��[�h�ƃV�F�[�_�[���\�[�X�r���[�I�u�W�F�N�g�̐���
    ID3D11Resource* resource = {}; // ���ׂẴ��\�[�X�ɋ��ʂ̃A�N�V�������
    hr = DirectX::CreateWICTextureFromFile(
        device,                 //
        filename,               // 
        &resource,              // 
        shaderResourceView.GetAddressOf()   // 
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    resource->Release();

    //�e�N�X�`�����̎擾
    ID3D11Texture2D* texture2d = {};
    hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    texture2d->GetDesc(&texture2dDesc);
    texture2d->Release();
}

// �`��
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

    immediateContext->VSSetShader(                                 // �f�o�C�X�ɒ��_�V�F�[�_��ݒ�
        vertexShader.Get(),                                              // ���_�V�F�[�_�ւ̃|�C���^ 
        nullptr,                                                    // 
        0                                                           // �z����̃N���X �C���X�^���X �C���^�[�t�F�C�X�̐�
    );
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
}
void SpriteBatch::End(ID3D11DeviceContext* immediateContext) {
    HRESULT hr = S_OK;
    D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
    hr = immediateContext->Map(    // �T�u���\�[�X�Ɋ܂܂��f�[�^�ւ̃|�C���^���擾���A���̃T�u���\�[�X�ւ�GPU�A�N�Z�X������
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
        0,                  // �o�C���h����ŏ��̓��̓X���b�g
        1,                  // �z����̒��_�o�b�t�@�[�̐�
        vertexBuffer.GetAddressOf(),     // ���_�o�b�t�@�[�̔z��ւ̃|�C���^
        &stride,            // �X�g���C�h�l�̔z��ւ̃|�C���^�[
        &offset             // �I�t�Z�b�g�̔z��ւ̃|�C���^
    );

    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //�v���~�e�B�u�^�Ɋւ�����A���̓A�Z���u���E�X�e�[�W���̓f�[�^���L�q����f�[�^�������o�C���h

    immediateContext->IASetInputLayout(inputLayout.Get());              // ���̓��C�A�E�g�E�I�u�W�F�N�g����̓A�Z���u���E�X�e�[�W�Ƀo�C���h


    immediateContext->Draw(static_cast<UINT>(vertexCount), 0);                                  // ���_�� �ŏ��̒��_�̃C���f�b�N�X
}

SpriteBatch::~SpriteBatch() {}