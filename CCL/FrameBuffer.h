#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

class FrameBuffer {
public:
    FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height);
    virtual ~FrameBuffer() = default;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[2];

    D3D11_VIEWPORT viewport;

    void Clear(ID3D11DeviceContext* dc,
        float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f, float depth = 1.0f);
    void Activate(ID3D11DeviceContext* dc);
    void DeActivate(ID3D11DeviceContext* dc);

private:
    UINT viewPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    D3D11_VIEWPORT cachedViewport[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
};