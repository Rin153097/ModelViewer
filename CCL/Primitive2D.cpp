#include "CCL.h"
#include "Primitive2D.h"

PrimitiveRect::PrimitiveRect(RenderSystem* renderSystem) {
    Vertex vertices[] = {
        {{-1.0f, +1.0f,0.0f},{1,1,1,1}},
        {{+1.0f, +1.0f,0.0f},{1,1,1,1}},
        {{-1.0f, -1.0f,0.0f},{1,1,1,1}},
        {{+1.0f, -1.0f,0.0f},{1,1,1,1}},
    };

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;

    HRESULT hr = S_OK;
    hr = renderSystem->device->CreateBuffer(
        &bufferDesc,
        &subresourceData,
        &vertexBuffer
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_INPUT_ELEMENT_DESC inputElement[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    CreateVsFromCso(renderSystem->device.Get(), "./resources/Shader/primitiveRectVS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.GetAddressOf(), inputElement, _countof(inputElement));
    CreatePsFromCso(renderSystem->device.Get(), "./resources/Shader/primitiveRectPS.cso", pixelShader.GetAddressOf());
}

void PrimitiveRect::Render(RenderSystem* renderSystem,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a, float radian) {
    ID3D11DeviceContext* dc = renderSystem->immediateContext.Get();

    renderSystem->SwitchDepthStencil(DEPTH_STATE::DISABLE_NOT_WRITE);
    renderSystem->SwitchRasterizer(RASTERIZER_STATE::FILL_NOT_CULL);


    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    dc->RSGetViewports(
        &num_viewports,
        &viewport
    );

    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    dc->IASetVertexBuffers(
        0,                  // �o�C���h����ŏ��̓��̓X���b�g
        1,                  // �z����̒��_�o�b�t�@�[�̐�
        vertexBuffer.GetAddressOf(),     // ���_�o�b�t�@�[�̔z��ւ̃|�C���^
        &stride,            // �X�g���C�h�l�̔z��ւ̃|�C���^�[
        &offset             // �I�t�Z�b�g�̔z��ւ̃|�C���^
    );

    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); //�v���~�e�B�u�^�Ɋւ�����A���̓A�Z���u���E�X�e�[�W���̓f�[�^���L�q����f�[�^�������o�C���h

    dc->IASetInputLayout(inputLayout.Get());              // ���̓��C�A�E�g�E�I�u�W�F�N�g����̓A�Z���u���E�X�e�[�W�Ƀo�C���h

    dc->VSSetShader(                                 // �f�o�C�X�ɒ��_�V�F�[�_��ݒ�
        vertexShader.Get(),                                              // ���_�V�F�[�_�ւ̃|�C���^ 
        nullptr,                                                    // 
        0                                                           // �z����̃N���X �C���X�^���X �C���^�[�t�F�C�X�̐�
    );
    dc->PSSetShader(pixelShader.Get(), nullptr, 0);


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
        Rotate(px[i], py[i], cx, cy, radian);
    }

    // �ϊ�
    for (int i = 0; i < verNum; ++i) {
        px[i] = 2.0f * px[i] / viewport.Width - 1.0f;
        py[i] = 1.0f - 2.0f * py[i] / viewport.Height;
    }

    HRESULT hr = S_OK;
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = dc->Map(    // �T�u���\�[�X�Ɋ܂܂��f�[�^�ւ̃|�C���^���擾���A���̃T�u���\�[�X�ւ�GPU�A�N�Z�X������
        vertexBuffer.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped_subresource
    );
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    Vertex* vertices = reinterpret_cast<Vertex*>(mapped_subresource.pData);
    if (vertices != nullptr) {
        for (int i = 0; i < 4; ++i) {
            vertices[i].position = { px[i],py[i],0 };
            vertices[i].color = { r,g,b,a };
        }
    }

    dc->Unmap(vertexBuffer.Get(), 0);

    dc->Draw(4, 0);                                  // ���_�� �ŏ��̒��_�̃C���f�b�N�X
}