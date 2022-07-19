#include "staticMesh.h"
#include "GeometricPrimitive.h"
#include "SpriteModule.h"

#include "misc.h"

#include "sprite.h"

#include <filesystem>

StaticMesh::StaticMesh(ID3D11Device* device, const wchar_t* objFileName, bool flipCoordV) {
    using namespace DirectX;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t currentIndex = 0;

    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;
    std::vector<std::wstring> mtlFileNames;

    std::wifstream fin(objFileName);
    _ASSERT_EXPR(fin, L"OBJ file not found.");
    wchar_t command[256];
    while (fin) {
        fin >> command;
        if (0 == wcscmp(command, L"v")) {
            float x, y, z;
            fin >> x >> y >> z;
            positions.push_back({ x,y,z });
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vn")) {
            float i, j, k;
            fin >> i >> j >> k;
            normals.push_back({ i,j,k });
            fin.ignore(1024, '\n');
        }
        else if (0 == wcscmp(command, L"f")) {
            for (size_t i = 0; i < 3; ++i) {
                Vertex vertex;
                size_t v, vt, vn;

                fin >> v;
                vertex.position = positions.at(v - 1);
                if (L'/' == fin.peek()) {
                    fin.ignore(1);
                    if (L'/' != fin.peek()) {
                        fin >> vt;
                        vertex.texcoord = texcoords.at(vt - 1);
                    }
                    if (L'/' == fin.peek()) {
                        fin.ignore(1);
                        fin >> vn;
                        vertex.normal = normals.at(vn - 1);
                    }
                }
                vertices.push_back(vertex);
                indices.push_back(currentIndex++);
            }
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vt")) {
            float u, v;
            fin >> u >> v;
            texcoords.push_back({ u , (flipCoordV ? 1.0f - v : v) });
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"mtllib")) {
            wchar_t mtllib[256];
            fin >> mtllib;
            mtlFileNames.push_back(mtllib);
        }
        else if (0 == wcscmp(command, L"usemtl")) {
            wchar_t usemtl[MAX_PATH] = {};
            fin >> usemtl;

            subsets.push_back({ usemtl, static_cast<uint32_t>(indices.size()), 0 });
        }
        else {
            fin.ignore(1024, L'\n');
        }
    }
    fin.close();

    std::vector<Subset>::reverse_iterator it = subsets.rbegin();
    it->indexCount = static_cast<uint32_t>(indices.size()) - it->indexStart;
    for (it = subsets.rbegin() + 1; it != subsets.rend(); ++it) {
        it->indexCount = (it - 1)->indexStart - it->indexStart;
    }

    std::filesystem::path mtlFileName(objFileName);
    mtlFileName.replace_filename(std::filesystem::path(mtlFileNames[0]).filename());

    fin.open(mtlFileName);
    _ASSERT_EXPR(fin, L"'MTL file not found.");

    while (fin)
    {
        fin >> command;

        if (0 == wcscmp(command, L"map_Kd"))
        {
            fin.ignore();

            wchar_t map_Kd[256];
            fin >> map_Kd;

            // テクスチャファイル名にパスを取り付ける
            std::filesystem::path path(objFileName);
            path.replace_filename(std::filesystem::path(map_Kd).filename());
            materials.rbegin()->textureFileName[0] = path;

            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"newmtl")) {
            fin.ignore();
            wchar_t newmtl[256];
            Material mat;
            fin >> newmtl;

            mat.name = newmtl;
            materials.push_back(mat);
        }
        else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump")) {
            fin.ignore();
            wchar_t map_bump[256];
            fin >> map_bump;

            std::filesystem::path path(objFileName);
            path.replace_filename(std::filesystem::path(map_bump).filename());
            materials.rbegin()->textureFileName[1] = path;
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"Kd")) {
            float r, g, b;
            fin >> r >> g >> b;
            materials.rbegin()->Kd = { r , g, b, 1 };
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"Ka")) {
            float r, g, b;
            fin >> r >> g >> b;
            materials.rbegin()->Ka = { r , g, b, 1 };
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"Ks")) {
            float r, g, b;
            fin >> r >> g >> b;
            materials.rbegin()->Ks = { r , g, b, 1 };
            fin.ignore(1024, L'\n');
        }
        else
        {
            fin.ignore(1024, L'\n');
        }
    }
    fin.close();

    CreateComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

    HRESULT hr = S_OK;

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    CreateVsFromCso(device, "./resources/Shader/staticMeshVS.cso", vertexShader.GetAddressOf(),
        inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    CreatePsFromCso(device, "./resources/Shader/staticMeshPS.cso", pixelShader.GetAddressOf());

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(constants);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    if (materials.size() == 0) {
        for (const Subset& s : subsets) {
            materials.push_back({ s.usemtl });
        }
    }

    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    for (Material& mat : materials) {
        if (mat.textureFileName[0].size() > 0) {
            LoadTextureFromFile(device, mat.textureFileName[0].c_str(), mat.shaderResourceView[0].GetAddressOf(), &texture2dDesc);
        }
        else {
            MakeDummyTexture(device, mat.shaderResourceView[0].GetAddressOf(), 0xffffffff, 16);
        }
        if (mat.textureFileName[1].size() > 0) {
            LoadTextureFromFile(device, mat.textureFileName[1].c_str(), mat.shaderResourceView[1].GetAddressOf(), &texture2dDesc);
        }
        else {
            MakeDummyTexture(device, mat.shaderResourceView[1].GetAddressOf(), 0xffff7f7f, 16);
        }
    }
}

void StaticMesh::CreateComBuffers(ID3D11Device* device, Vertex* vertices, size_t vertexCount,
    uint32_t* indices, size_t indexCount) {
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertexCount);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indexCount);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    subresourceData.pSysMem = indices;

    hr = device->CreateBuffer(&bufferDesc, &subresourceData, indexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void StaticMesh::render(ID3D11DeviceContext* immediateContext,
    const Matrix& world, const Vec4& materialColor) {
    uint32_t stride = sizeof(Vertex);
    uint32_t offset = 0;

    immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediateContext->IASetInputLayout(inputLayout.Get());

    immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    for (const Material& mat : materials) {
        immediateContext->PSSetShaderResources(0, 1, mat.shaderResourceView[0].GetAddressOf());
        immediateContext->PSSetShaderResources(1, 1, mat.shaderResourceView[1].GetAddressOf());

        constants data = { world,materialColor };
        immediateContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
        immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

        for (const Subset& s : subsets) {
            if (mat.name == s.usemtl) {
                immediateContext->DrawIndexed(s.indexCount, s.indexStart, 0);
            }
        }
    }
}