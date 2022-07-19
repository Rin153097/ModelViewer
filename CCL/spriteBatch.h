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
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader;  // ���_�V�F�[�_�[�X�e�[�W�𐧌䂷����s�\�v���O�������Ǘ�
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader;    // ���l�ɂ�����̓s�N�Z���V�F�[�_�[
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout;    // ���_�f�[�^���A�Z���u���[�X�e�[�W�ɑ��荞�ޕ��@���`
    Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer;        // ��\�����������ł���o�b�t�@�[ ���\�[�X�ɃA�N�Z�X �o�b�t�@�͒ʏ�A���_�f�[�^�܂��̓C���f�b�N�X �f�[�^���i�[

    // about sprite
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView; // �����_�����O���ɃV�F�[�_���A�N�Z�X�ł���T�u���\�[�X���w�� (�V�F�[�_�[���\�[�X�@�萔�o�b�t�@�E�e�N�X�`���o�b�t�@�A�e�N�X�`��)
    D3D11_TEXTURE2D_DESC texture2dDesc; // 2D�e�N�X�`���ɂ��Đ���

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