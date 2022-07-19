#pragma once

#include "Vector.h"
#include "OffScreenWindow.h"

#include <d3d11.h>
#include "misc.h"
#include <wrl.h>
#include <vector>
#include <memory>

#include "RenderStates.h"
#include "FrameBuffer.h"
#include "FullScreenQuad.h"

class SamplerStates {
public:
    SamplerStates(ID3D11Device* device);
    SamplerStates() {}
private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[static_cast<int>(SAMPLER_STATE::MAX)];

public:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> GetComPtr(SAMPLER_STATE index) { return samplerStates[static_cast<int>(index)]; }
    ID3D11SamplerState* Get(SAMPLER_STATE index) { return samplerStates[static_cast<int>(index)].Get(); }
    ID3D11SamplerState** GetAddressOf(SAMPLER_STATE index) { return samplerStates[static_cast<int>(index)].GetAddressOf(); }
    ID3D11SamplerState** ReleaseAndGetAddressOf(SAMPLER_STATE index) { return samplerStates[static_cast<int>(index)].ReleaseAndGetAddressOf(); }
};
class DepthStencilStates {
public:
    DepthStencilStates(ID3D11Device* device);
    DepthStencilStates() {}
private:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[static_cast<int>(DEPTH_STATE::MAX)];
public:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetComPtr(DEPTH_STATE index) { return depthStencilStates[static_cast<int>(index)]; }
    ID3D11DepthStencilState* Get(DEPTH_STATE index) { return depthStencilStates[static_cast<int>(index)].Get(); }
    ID3D11DepthStencilState** GetAddressOf(DEPTH_STATE index) { return depthStencilStates[static_cast<int>(index)].GetAddressOf(); }
    ID3D11DepthStencilState** ReleaseAndGetAddressOf(DEPTH_STATE index) { return depthStencilStates[static_cast<int>(index)].ReleaseAndGetAddressOf(); }
};
class BlendStates {
public:
    BlendStates(ID3D11Device* device);
    BlendStates() {}
private:
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[static_cast<int>(BLEND_TYPE::MAX)];
public:
    Microsoft::WRL::ComPtr<ID3D11BlendState> GetComPtr(BLEND_TYPE index) { return blendStates[static_cast<int>(index)]; }
    ID3D11BlendState* Get(BLEND_TYPE index) { return blendStates[static_cast<int>(index)].Get(); }
    ID3D11BlendState** GetAddressOf(BLEND_TYPE index) { return blendStates[static_cast<int>(index)].GetAddressOf(); }
    ID3D11BlendState** ReleaseAndGetAddressOf(BLEND_TYPE index) { return blendStates[static_cast<int>(index)].ReleaseAndGetAddressOf(); }
};
class RasterizerStates {
public:
    RasterizerStates(ID3D11Device* device);
    RasterizerStates() {}
private:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[static_cast<int>(RASTERIZER_STATE::MAX)];
public:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetComPtr(RASTERIZER_STATE index) { return rasterizerStates[static_cast<int>(index)]; }
    ID3D11RasterizerState* Get(RASTERIZER_STATE index) { return rasterizerStates[static_cast<int>(index)].Get(); }
    ID3D11RasterizerState** GetAddressOf(RASTERIZER_STATE index) { return rasterizerStates[static_cast<int>(index)].GetAddressOf(); }
    ID3D11RasterizerState** ReleaseAndGetAddressOf(RASTERIZER_STATE index) { return rasterizerStates[static_cast<int>(index)].ReleaseAndGetAddressOf(); }
};

class RenderSettings {
public:
    RenderSettings() {

    }
    RenderSettings(ID3D11Device* device) :
        samplerStates(SamplerStates(device)),
        depthStencilStates(DepthStencilStates(device)),
        blendStates(BlendStates(device)),
        rasterizerStates(RasterizerStates(device)) {}
public:
    SamplerStates       samplerStates;
    DepthStencilStates  depthStencilStates;
    BlendStates         blendStates;
    RasterizerStates    rasterizerStates;
};

class SceneConstants {
public:
    struct Data {
        Matrix	viewProjection;
        Vec4	lightDirection;
        Vec4    ambientLight;
        Vec4	cameraPosition;

        Data() :viewProjection(), lightDirection(Vec4(0.0f, 0.0f, 1.0f, 0.0f)), ambientLight(), cameraPosition(0.0f, 0.0f, 0.0f, 0.0f) {}
    };
public:
    SceneConstants() :constantBufferData(), constantBuffers() {}

private:
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> constantBuffers;
    std::vector<Data> constantBufferData;

public:
    ID3D11Buffer* GetBuffer(int index) { return constantBuffers.at(index).Get(); }
    ID3D11Buffer** GetAddressOfBuffer(int index) { return constantBuffers.at(index).GetAddressOf(); }
    ID3D11Buffer** ReleaseAndGetAddressOfBuffer(int index) { return constantBuffers.at(index).ReleaseAndGetAddressOf(); }

    Data* GetBufferData(int index) { return &constantBufferData.at(index); }

    void AddConstantBuffer(size_t incrementNum) {
        for (size_t i = 0; i < incrementNum; ++i) {
            constantBuffers.emplace_back();
            constantBufferData.emplace_back();
        }
    }
};

class RenderSystem {
public:
    RenderSystem() {}
    RenderSystem(UINT screenWidth, UINT screenHeight, HWND hwnd, bool fullScreen);
public:
    Microsoft::WRL::ComPtr<ID3D11Device>		    device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     immediateContext;

    Microsoft::WRL::ComPtr<IDXGISwapChain>		    swapChain;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  depthStencilView;

    // off Screen
    std::unique_ptr<FrameBuffer>                    frameBuffers[8];
    std::unique_ptr<FullScreenQuad>                 bitBlockTransfer;
    std::unique_ptr<OffScreenWindow>                bitBlockWindow;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>       pixelShaders[8];

    RenderSettings                                  renderSettings;

    SceneConstants                                  constantBuffer;

private:
    void InitDevice_DeviceContext_SwapChain(UINT screenWidth, UINT screenHeight, HWND hwnd, bool fullScreen);
    void InitRenderTargetView();
    void InitViewPort(UINT screenWidth, UINT screenHeight);
    void Init2dTexture(UINT screenWidth, UINT screenHeight);

    DXGI_SWAP_CHAIN_DESC MakeSwapChainDesk(UINT screenWidth, UINT screenHeight, HWND hwnd, bool fullScreen);

public:
    void SetupRender(Vec4 color = { 1.0f }, int bufferIndex = 0) {
        ClearScreen(color);
        UpdateSubresourceData(bufferIndex);
        SetVSConstantBuffer(bufferIndex);
        SetPSConstantBuffer(bufferIndex);
    }
public:
    void ClearScreen(Vec4 color = { 1.0f }) {
        ID3D11RenderTargetView* nullRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
        immediateContext->OMSetRenderTargets(_countof(nullRenderTargetViews), nullRenderTargetViews, 0);
        ID3D11ShaderResourceView* nullShaderResourceViews[D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
        immediateContext->VSSetShaderResources(0, _countof(nullShaderResourceViews), nullShaderResourceViews);
        immediateContext->PSSetShaderResources(0, _countof(nullShaderResourceViews), nullShaderResourceViews);

        FLOAT c[] = { color.x,color.y,color.z,color.w };
        immediateContext->ClearRenderTargetView(renderTargetView.Get(), c);	// レンダーターゲットビューのすべての要素を一つの値に集約

        ClearDepth();
        SetRenderTarget();
    }

private:
    void ClearDepth() {                                 // ユーザーの認知は必要ない
        immediateContext->ClearDepthStencilView(
            depthStencilView.Get(),						// 深度ステンシルリソースのクリア 深度ステンシルへのポインタ
            D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,	// クリアするデータの種類を指定 今回は深度もステンシルもクリア
            1.0f,										// この値で深度バッファーをクリア clamp(0 ~ 1);
            0											// この値でステンシルバッファーをクリア
        );
    }
    void SetRenderTarget() {
        immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());	// レンダーターゲットと深度ステンシルのCPU記述しハンドルを設定
    }


public:
    void SwitchRasterizer(RASTERIZER_STATE index) {
        immediateContext->RSSetState(renderSettings.rasterizerStates.Get(index));
    }
    void SwitchDepthStencil(DEPTH_STATE index) {
        immediateContext->OMSetDepthStencilState(renderSettings.depthStencilStates.Get(index), 1);
    }
    void SwitchBlendState(BLEND_TYPE index) {
        immediateContext->OMSetBlendState(renderSettings.blendStates.Get(index), nullptr, 0xFFFFFFFF);
    }

public:
    void UpdateSubresourceData(int index);
    void SetVSConstantBuffer(int index);
    void SetPSConstantBuffer(int index);

    void SetConstantViewProjection  (Matrix viewProjection, int index)  { constantBuffer.GetBufferData(index)->viewProjection   = viewProjection; }
    void SetConstantCameraPosition  (Vec3 cameraPosition,   int index)  { constantBuffer.GetBufferData(index)->cameraPosition   = cameraPosition.CastToVec4(); }
    void SetConstantAmbientLight    (Vec4 ambientLight,     int index)  { constantBuffer.GetBufferData(index)->ambientLight     = ambientLight; }
    void SetConstantLightDirection  (Vec3 lightDirection,   int index)  { constantBuffer.GetBufferData(index)->lightDirection   = lightDirection.CastToVec4(); }
};