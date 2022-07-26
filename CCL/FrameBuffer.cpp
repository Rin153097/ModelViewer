#include "FrameBuffer.h"
#include "misc.h"

FrameBuffer::FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height) {
    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    texture2dDesc.Width = width;
    texture2dDesc.Height = height;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture2dDesc.CPUAccessFlags = 0;

    hr = device->CreateTexture2D(&texture2dDesc, 0, renderTargetBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
    renderTargetViewDesc.Format = texture2dDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = device->CreateRenderTargetView(renderTargetBuffer.Get(), &renderTargetViewDesc, renderTargetView.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
    shaderResourceViewDesc.Format = texture2dDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(renderTargetBuffer.Get(), 
        &shaderResourceViewDesc, shaderResourceViews[0].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
    texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texture2dDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    hr = device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = 0;
    hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc,
        depthStencilView.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
}
void FrameBuffer::Clear(ID3D11DeviceContext* dc,
    float r, float g, float b, float a, float depth) {
    float color[4] = { r, g, b, a };
    dc->ClearRenderTargetView(renderTargetView.Get(), color);
    dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

void FrameBuffer::Activate(ID3D11DeviceContext* dc) {
    viewPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    dc->RSGetViewports(&viewPortCount, cachedViewport);
    dc->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(),
        cachedDepthStencilView.ReleaseAndGetAddressOf());

    dc->RSSetViewports(1, &viewport);
    dc->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}
void FrameBuffer::DeActivate(ID3D11DeviceContext* dc) {
    dc->RSSetViewports(viewPortCount, cachedViewport);
    dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}