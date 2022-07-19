#include "SpriteModule.h"
#include "misc.h"

#include <wrl.h>
#include <map>

#include <memory>

#include <filesystem>

#include <dxgidebug.h>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

static map<wstring, ComPtr<ID3D11ShaderResourceView>> resources;


// #define CheckDeviceContext

HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename,
    ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesk) {
    HRESULT hr = S_OK;
    ComPtr<ID3D11Resource> resource;

    auto it = resources.find(filename);
    if (it != resources.end()) {
        *shaderResourceView = it->second.Get();
        (*shaderResourceView)->AddRef();
        (*shaderResourceView)->GetResource(resource.GetAddressOf());
    }
    else {
        std::filesystem::path changeExFileName(filename);
        changeExFileName.replace_extension("dds");
        if (std::filesystem::exists(changeExFileName.c_str())) {
            ComPtr<ID3D11DeviceContext> immediateContext;
            device->GetImmediateContext(immediateContext.GetAddressOf());
            hr = DirectX::CreateDDSTextureFromFile(
                device, immediateContext.Get(), changeExFileName.c_str(),
                resource.GetAddressOf(), shaderResourceView);
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        }
        else  {
            changeExFileName.replace_extension("tga");
            if (std::filesystem::exists(changeExFileName.c_str())) {
                TexMetadata meta;
                ComPtr<ID3D11DeviceContext> immediateContext;
                std::unique_ptr<ScratchImage> image(new ScratchImage);
                device->GetImmediateContext(immediateContext.GetAddressOf());

                hr = DirectX::LoadFromTGAFile(changeExFileName.c_str(),
                    &meta, *image.get());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
                hr = CreateShaderResourceView(
                    device, image->GetImages(), image->GetImageCount(), meta, shaderResourceView);
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else {
                changeExFileName.replace_extension("png");
                if (std::filesystem::exists(changeExFileName.c_str())) {
                    hr = DirectX::CreateWICTextureFromFile(
                        device, changeExFileName.c_str(), resource.GetAddressOf(), shaderResourceView);
                    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
                }
                else {
                    return E_FAIL;
                }
            }
            resources.insert(make_pair(filename, *shaderResourceView));
        }
    }
    /*
    ComPtr<ID3D11Texture2D> texture2d;
    hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    texture2d->GetDesc(texture2dDesk);
    */

    return hr;
}
void relearse_all_textures() {
    resources.clear();
}

HRESULT CreateVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader,
    ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements) {
    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File dont found");

    fseek(fp, 0, SEEK_END);
    long csoSz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unique_ptr<unsigned char[]> csoData = make_unique<unsigned char[]>(csoSz);
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    HRESULT hr = S_OK;
    hr = device->CreateVertexShader(csoData.get(), csoSz, nullptr, vertexShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    if (inputLayout) {
        hr = device->CreateInputLayout(inputElementDesc, numElements,
            csoData.get(), csoSz, inputLayout);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    return hr;
}

HRESULT CreatePsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader) {
    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File dont found");

    fseek(fp, 0, SEEK_END);
    long csoSz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unique_ptr<unsigned char[]> csoData = make_unique<unsigned char[]>(csoSz);
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    HRESULT hr = S_OK;
    hr = device->CreatePixelShader(csoData.get(), csoSz, nullptr, pixelShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}