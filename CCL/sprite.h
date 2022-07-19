#pragma once

#include "misc.h"
#include <d3d11.h>
#include "Vector.h"

#include <wrl.h>

class sprite {
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>          vertexShader;       // ���_�V�F�[�_�[�X�e�[�W�𐧌䂷����s�\�v���O�������Ǘ�
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           pixelShader;        // ���l�ɂ�����̓s�N�Z���V�F�[�_�[
    Microsoft::WRL::ComPtr<ID3D11InputLayout>           inputLayout;        // ���_�f�[�^���A�Z���u���[�X�e�[�W�ɑ��荞�ޕ��@���`
    Microsoft::WRL::ComPtr<ID3D11Buffer>                vertexBuffer;       // ��\�����������ł���o�b�t�@�[ ���\�[�X�ɃA�N�Z�X �o�b�t�@�͒ʏ�A���_�f�[�^�܂��̓C���f�b�N�X �f�[�^���i�[

    // about sprite
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    shaderResourceView; // �����_�����O���ɃV�F�[�_���A�N�Z�X�ł���T�u���\�[�X���w�� (�V�F�[�_�[���\�[�X�@�萔�o�b�t�@�E�e�N�X�`���o�b�t�@�A�e�N�X�`��)
    // Microsoft::WRL::ComPtr<D3D11_TEXTURE2D_DESC>        texture2dDesc;      // 2D�e�N�X�`���ɂ��Đ���
    D3D11_TEXTURE2D_DESC        texture2dDesc;      // 2D�e�N�X�`���ɂ��Đ���

public:
    void render(ID3D11DeviceContext* immediateContext,
        float dx, float dy, float dw, float dh,
        float r, float g, float b, float a,float angle);

    void render(ID3D11DeviceContext* immediateContext,
        float dx, float dy, float dw, float dh,
        float r, float g, float b, float a,
        float angle,
        float sx, float sy, float sw, float sh);

    sprite(ID3D11Device* device,const wchar_t* filename);
    ~sprite();

    UINT Width()   { return texture2dDesc.Width; }
    UINT Height()  { return texture2dDesc.Height; }

    struct vertex {
        Vec3 position;
        Vec4 color;
        Vec2 texcoord;
    };

    
};

HRESULT MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shaderResourceView,
    DWORD value, UINT dimension);