#include "misc.h"
#include "SkinnedMesh.h"
#include "SpriteModule.h"
#include "sprite.h"

#include <fstream>
#include <sstream>
#include <functional>
#include <filesystem>

#include "FbxVectorConvert.h"

struct BoneInfluence {
    uint32_t boneIndex;
    float boneWeight;
};
using BoneInfluencePerControlPoint = std::vector<BoneInfluence>;

void FetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<BoneInfluencePerControlPoint>& boneInfluences) {
    const int controlPointsCount = fbxMesh->GetControlPointsCount();
    boneInfluences.resize(controlPointsCount);

    const int skinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex) {
        const FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin));

        const int clusterCount = fbxSkin->GetClusterCount();
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex) {
            const FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIndex);

            int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
            for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex) {
                int controlPointIndex = fbxCluster->GetControlPointIndices()[controlPointIndicesIndex];
                double controlPointWeight = fbxCluster->GetControlPointWeights()[controlPointIndicesIndex];
                BoneInfluence& boneInfluence = boneInfluences.at(controlPointIndex).emplace_back();
                boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
                boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
            }
        }
    }
}

SkinMesh::SkinMesh(std::vector<Animation>* animations, ID3D11Device* device, const char* fbxFileName, float samplingRate, bool triangleate,
    COORDINATE_TYPE coordinateType,
    RASTERIZER_STATE rs, BLEND_TYPE bt, DEPTH_STATE ds,
    Vec4 forceAmbientColor, bool serialize) :
    rasterizerState(rs), blendType(bt), depthState(ds), forceAmbientColor(forceAmbientColor)
{
    std::filesystem::path cerealFileName(fbxFileName);
    cerealFileName.replace_extension("cereal");
    if (std::filesystem::exists(cerealFileName.c_str())) {
        std::ifstream ifs(cerealFileName.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        if (animations) deserialization(sceneView, meshes, materials, *animations);
        else {
            AnimationManager NullAnimation;
            deserialization(sceneView, meshes, materials, NullAnimation.animations);
        }

    }
    else {
        bool exist =  std::filesystem::exists(fbxFileName);
        FbxManager* fbxManager = FbxManager::Create();
        FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
        fbxManager->SetIOSettings(ios);
        FbxScene* fbxScene = FbxScene::Create(fbxManager, "Scene");

        FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

        bool importStatus = false;
        importStatus = fbxImporter->Initialize(fbxFileName);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        importStatus = fbxImporter->Import(fbxScene);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        FbxGeometryConverter fbxConverter(fbxManager);

        fbxsdk::FbxAxisSystem::EUpVector    upVec = (static_cast<int>(coordinateType) % 4) <= 1 ? fbxsdk::FbxAxisSystem::EUpVector::eYAxis : fbxsdk::FbxAxisSystem::EUpVector::eZAxis;
        fbxsdk::FbxAxisSystem::EFrontVector frontVec = fbxsdk::FbxAxisSystem::EFrontVector::eParityOdd;
        fbxsdk::FbxAxisSystem::ECoordSystem hand = (static_cast<int>(coordinateType) % 2) ? fbxsdk::FbxAxisSystem::ECoordSystem::eLeftHanded : fbxsdk::FbxAxisSystem::ECoordSystem::eRightHanded;

        fbxsdk::FbxAxisSystem axis(
            upVec,
            frontVec,
            hand
        );
        axis.ConvertScene(fbxScene);

        if (triangleate) {
            fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
            fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
        }

        std::function<void(FbxNode*)> traverse =
            [&](FbxNode* fbxNode) {
            Scene::Node& node = sceneView.nodes.emplace_back();
            node.attribute = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
            node.name = fbxNode->GetName();
            node.uniqueID = fbxNode->GetUniqueID();
            node.parentIndex = sceneView.indexof(fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0);
            for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex) {
                traverse(fbxNode->GetChild(childIndex));
            }
        };
        traverse(fbxScene->GetRootNode());
#if 0
        for (const Scene::Node& node : sceneView.nodes) {
            FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());

            std::string nodeName = fbxNode->GetName();
            uint64_t uid = fbxNode->GetUniqueID();
            uint64_t parent_uid = fbxNode->GetParent() ?
                fbxNode->GetParent()->GetUniqueID() : 0;
            int32_t type = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : 0;

            std::stringstream debugString;
            debugString << nodeName << ":" << uid << ":" << parent_uid << ":" << type << '\n';
            OutputDebugStringA(debugString.str().c_str());
        }

#endif

        FetchMeshes(fbxScene, meshes);
        FetchMaterials(fbxScene, materials);
        if (animations) FetchAnimation(fbxScene, *animations, samplingRate);
        fbxManager->Destroy();

        if (serialize) {
            Serialization(fbxFileName, animations);
        }
    }
    CreateComObjects(device, fbxFileName);
}
void SkinMesh::FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes) {
    for (const Scene::Node& node : sceneView.nodes) {
        if (node.attribute != FbxNodeAttribute::EType::eMesh) {
            continue;
        }
        FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
        FbxMesh* fbxMesh = fbxNode->GetMesh();

        Mesh& mesh = meshes.emplace_back();
        mesh.uniqueID = fbxMesh->GetNode()->GetUniqueID();
        mesh.name = fbxMesh->GetNode()->GetName();
        mesh.nodeIndex = sceneView.indexof(mesh.uniqueID);


        mesh.defaultGlobalTransform = FbxToMatrix(fbxMesh->GetNode()->EvaluateGlobalTransform());

        std::vector<BoneInfluencePerControlPoint> boneInfluences;

        FetchBoneInfluences(fbxMesh, boneInfluences);
        FetchSkelton(fbxMesh, mesh.bindPose);

        std::vector<Mesh::Subset>& subsets = mesh.subsets;
        const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
        subsets.resize(materialCount > 0 ? materialCount : 1);
        for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {

            const FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
            subsets.at(materialIndex).materialName = fbxMaterial->GetName();
            subsets.at(materialIndex).materialUniqueID = fbxMaterial->GetUniqueID();
        }
        if (materialCount > 0) {
            const int polygonCount = fbxMesh->GetPolygonCount();
            for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
                const int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
                subsets.at(materialIndex).indexCount += 3;
            }
            uint32_t offset = 0;
            for (Mesh::Subset& subset : subsets) {
                subset.startIndexLocation = offset;
                offset += subset.indexCount;

                subset.indexCount = 0;
            }
        }

        const int polygonCount = fbxMesh->GetPolygonCount();
        mesh.vertices.resize(polygonCount * 3LL);
        mesh.indices.resize(polygonCount * 3LL);

        FbxStringList uvNames;
        fbxMesh->GetUVSetNames(uvNames);
        const FbxVector4* controlPoints = fbxMesh->GetControlPoints();

        for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
            const int materialIndex = materialCount > 0 ?
                fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0;
            Mesh::Subset& subset = subsets.at(materialIndex);
            const uint32_t offset = subset.startIndexLocation + subset.indexCount;

            for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon) {
                const int vertexIndex = polygonIndex * 3 + positionInPolygon;

                Vertex vertex;
                const int polygonVertex = fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon);
                vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
                vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
                vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);

                if (fbxMesh->GenerateTangentsData(0, false)) {
                    const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
                    vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
                    vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
                    vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
                    vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
                }

                const BoneInfluencePerControlPoint& influencePerControlPoint = boneInfluences.at(polygonVertex);
                for (size_t influenceIndex = 0; influenceIndex < influencePerControlPoint.size(); ++influenceIndex) {
                    if (influenceIndex < MAX_BONE_INFLUENCES) {
                        vertex.boneWeight[influenceIndex] = influencePerControlPoint.at(influenceIndex).boneWeight;
                        vertex.boneIndices[influenceIndex] = influencePerControlPoint.at(influenceIndex).boneIndex;
                    }
                }


                if (fbxMesh->GetElementNormalCount() > 0) {
                    FbxVector4 normal;
                    fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
                    vertex.normal.x = static_cast<float>(normal[0]);
                    vertex.normal.y = static_cast<float>(normal[1]);
                    vertex.normal.z = static_cast<float>(normal[2]);
                }
                if (fbxMesh->GetElementUVCount() > 0) {
                    FbxVector2 uv;
                    bool unmappedUV;
                    fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmappedUV);
                    vertex.texCoord.x = static_cast<float>(uv[0]);
                    vertex.texCoord.y = 1.0f - static_cast<float>(uv[1]);
                }
                mesh.vertices.at(vertexIndex) = std::move(vertex);
                mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
                ++subset.indexCount;
            }
        }

        for (const Vertex& v : mesh.vertices) {
            mesh.boundingBox[0].x = std::min<float>(mesh.boundingBox[0].x, v.position.x);
            mesh.boundingBox[0].y = std::min<float>(mesh.boundingBox[0].y, v.position.y);
            mesh.boundingBox[0].z = std::min<float>(mesh.boundingBox[0].z, v.position.z);
            mesh.boundingBox[1].x = std::max<float>(mesh.boundingBox[1].x, v.position.x);
            mesh.boundingBox[1].y = std::max<float>(mesh.boundingBox[1].y, v.position.y);
            mesh.boundingBox[1].z = std::max<float>(mesh.boundingBox[1].z, v.position.z);
        }

    }
}
void SkinMesh::FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials) {
    const size_t nodeCount = sceneView.nodes.size();
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
        const Scene::Node& node = sceneView.nodes.at(nodeIndex);
        const FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());

        const int materialCount = fbxNode->GetMaterialCount();
        for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
            const FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(materialIndex);

            Material material;
            material.name = fbxMaterial->GetName();
            material.uniqueID = fbxMaterial->GetUniqueID();
            fbxsdk::FbxProperty fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

            if (fbxProperty.IsValid()) {
                const fbxsdk::FbxDouble3 color = fbxProperty.Get<fbxsdk::FbxDouble3>();
                material.Kd.x = static_cast<float>(color[0]);
                material.Kd.y = static_cast<float>(color[1]);
                material.Kd.z = static_cast<float>(color[2]);
                material.Kd.w = 1.0f;

                const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
                material.textureFileName[0] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
            }
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbxProperty.IsValid()) {
                const fbxsdk::FbxDouble3 color = fbxProperty.Get<fbxsdk::FbxDouble3>();
                material.Ks.x = static_cast<float>(color[0]);
                material.Ks.y = static_cast<float>(color[1]);
                material.Ks.z = static_cast<float>(color[2]);
                material.Ks.w = 1.0f;
                const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
                material.textureFileName[2] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
            }
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbxProperty.IsValid()) {
                const fbxsdk::FbxDouble3 color = fbxProperty.Get<fbxsdk::FbxDouble3>();
                material.Ka.x = static_cast<float>(color[0]);
                material.Ka.y = static_cast<float>(color[1]);
                material.Ka.z = static_cast<float>(color[2]);
                material.Ka.w = 1.0f;
            }
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbxProperty.IsValid()) {
                const FbxFileTexture* fileTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
                material.textureFileName[1] = fileTexture ? fileTexture->GetRelativeFileName() : "";
            }

            materials.emplace(material.uniqueID, std::move(material));
        }
    }
#if 1
    if (materials.size() == 0) {
        Material material;
        materials.emplace(material.uniqueID, std::move(material));
    }
#endif
}
void SkinMesh::FetchSkelton(FbxMesh* fbxMesh, Skelton& bindPose) {
    const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex) {
        FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
        const int clusterCount = skin->GetClusterCount();
        bindPose.bones.resize(clusterCount);
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex) {
            FbxCluster* cluster = skin->GetCluster(clusterIndex);

            Skelton::Bone& bone = bindPose.bones.at(clusterIndex);
            bone.name = cluster->GetLink()->GetName();
            bone.uniqueID = cluster->GetLink()->GetUniqueID();
            bone.parentIndex = bindPose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
            bone.nodeIndex = sceneView.indexof(bone.uniqueID);

            FbxAMatrix referenceGlobalInitPosition;
            cluster->GetTransformMatrix(referenceGlobalInitPosition);

            FbxAMatrix clusterGlobalInitPosition;
            cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

            bone.offsetTransform = FbxToMatrix(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
            int dammi = 0;
        }
    }
}

void SkinMesh::FetchAnimation(FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate)
{
    FbxArray<FbxString*> animationStackNames;
    fbxScene->FillAnimStackNameArray(animationStackNames);

    const int animationStackCount = animationStackNames.GetCount();

    for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
        Animation& animationClip = animationClips.emplace_back();

        animationClip.name = animationStackNames[animationStackIndex]->Buffer();

        FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str());
        fbxScene->SetCurrentAnimationStack(animationStack);

        const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
        FbxTime oneSecond;
        oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
        animationClip.samplingRate = samplingRate > 0 ?
            samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));
        const FbxTime samplingInterval =
            static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate);
        const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animationClip.name.c_str());
        const FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
        const FbxTime stopTime = takeInfo->mLocalTimeSpan.GetStop();
        for (FbxTime time = startTime; time < stopTime; time += samplingInterval) {
            Animation::KeyFrame& keyframe = animationClip.sequence.emplace_back();

            const size_t nodeCount = sceneView.nodes.size();
            keyframe.nodes.resize(nodeCount);
            for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
                FbxNode* fbxNode = fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str());
                if (fbxNode) {
                    Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

                    node.globalTransform = FbxToMatrix(fbxNode->EvaluateGlobalTransform(time));

                    const FbxAMatrix& localTransform = fbxNode->EvaluateLocalTransform(time);
                    node.scaling = FbxToVec3(localTransform.GetS());
                    node.rotationQ = FbxToVec4(localTransform.GetQ());
                    node.translation = FbxToVec3(localTransform.GetT());
                }
            }
        }
    }
    for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
        delete animationStackNames[animationStackIndex];
    }
}
void SkinMesh::UpdateAnimation(Animation::KeyFrame& keyframe, const SkinMesh& mesh) {
    using namespace DirectX;

    size_t nodeCount = keyframe.nodes.size();

    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
        Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

        // get nodeData
        XMMATRIX S = XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z);
        XMMATRIX R = XMMatrixRotationQuaternion(node.rotationQ.GetXM());
        XMMATRIX T = XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z);

        int64_t parentIndex = mesh.sceneView.nodes.at(nodeIndex).parentIndex;

        // update globaltransform by nodeData
        // if have parent, [ this transform * parent transform ]
        XMMATRIX P = parentIndex < 0 ?
            XMMatrixIdentity()
            : keyframe.nodes.at(parentIndex).globalTransform.GetXM();
        XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
    }
}
void SkinMesh::CreateComObjects(ID3D11Device* device, const char* fbxFileName) {
    for (Mesh& mesh : meshes) {
        HRESULT hr = S_OK;
        D3D11_BUFFER_DESC bufferDesc = {};
        D3D11_SUBRESOURCE_DATA subresourceData = {};

        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        subresourceData.pSysMem = mesh.vertices.data();
        subresourceData.SysMemPitch = 0;
        subresourceData.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&bufferDesc, &subresourceData,
            mesh.vertexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        subresourceData.pSysMem = mesh.indices.data();

        hr = device->CreateBuffer(&bufferDesc, &subresourceData,
            mesh.indexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#if 0
        mesh.vertices.clear();
        mesh.indices.clear();
#endif
    }
    for (std::unordered_map<uint64_t, Material>::iterator it = materials.begin(); it != materials.end(); ++it) {
        for (size_t textureIndex = 0; textureIndex < 4; ++textureIndex) {
            if (it->second.textureFileName[textureIndex].size() > 0) {

                std::filesystem::path path(fbxFileName);
                std::string filename = fbxFileName;
                // FixFileName(it->second.textureFileName[textureIndex], filename);
                path.replace_filename(it->second.textureFileName[textureIndex]);
                D3D11_TEXTURE2D_DESC texture2dDesc;

                if (LoadTextureFromFile(device, path.c_str(),
                    it->second.shaderResourceViews[textureIndex].GetAddressOf(), &texture2dDesc) == E_FAIL) {
                    MakeDummyTexture(device, it->second.shaderResourceViews[textureIndex].GetAddressOf(), 0xffffffff, 16);
                }
            }
            else {
                MakeDummyTexture(device, it->second.shaderResourceViews[textureIndex].GetAddressOf(), 0xffffffff, 16);
            }
        }
    }

    HRESULT hr = S_OK;
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POSITION",   0,  DXGI_FORMAT_R32G32B32_FLOAT,    0,  D3D11_APPEND_ALIGNED_ELEMENT},
        { "NORMAL",     0,  DXGI_FORMAT_R32G32B32_FLOAT,    0,  D3D11_APPEND_ALIGNED_ELEMENT},
        { "TANGENT",    0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT},
        { "TEXCOORD",   0,  DXGI_FORMAT_R32G32_FLOAT,       0,  D3D11_APPEND_ALIGNED_ELEMENT},
        { "WEIGHT",     0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT},
        { "BONES",      0,  DXGI_FORMAT_R32G32B32A32_UINT,  0,  D3D11_APPEND_ALIGNED_ELEMENT},
    };
    CreateVsFromCso(device, "./resources/Shader/skinnedMeshVS.cso", vertexShader.ReleaseAndGetAddressOf(),
        inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    CreatePsFromCso(device, "./resources/Shader/skinnedMeshPS.cso", pixelShader.ReleaseAndGetAddressOf());

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(Constants);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
bool SkinMesh::AppendAnimations(std::vector<Animation>& animations, const char* animationFileName, float samplingRate) {
    FbxManager* fbxManager = FbxManager::Create();
    FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

    FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
    bool importStatus = false;

    importStatus = fbxImporter->Initialize(animationFileName);
    _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
    importStatus = fbxImporter->Import(fbxScene);
    _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

    FetchAnimation(fbxScene, animations, samplingRate);

    fbxManager->Destroy();

    return true;
}

void SkinMesh::FixFileName(std::string& filepath, std::string& fbxFilename) {
    size_t filepathSize = filepath.size() - 1;
    // get File[name]
    std::string filename;
    for (size_t i = filepathSize;  i <= filepathSize + 1; --i) {
        if (filepath[i] == '\\' || filepath[i] == '/') {
            filename = filepath.erase(0, i + 1);
            break;
        }
    }
    _ASSERT_EXPR(filename.size(), "filename not found");

    // get folder.fbm
    size_t filenameSize = fbxFilename.size() - 1;
    std::string foldername;
    for (size_t i = filenameSize; i <= filenameSize + 1; --i) {
        if (fbxFilename[i] == '\\' || fbxFilename[i] == '/') {
            foldername = fbxFilename.erase(0, i + 1);
            break;
        }
    }
    std::string nonExFoldername;
    size_t foldernameSize = foldername.size() - 1;
    for (size_t i = foldernameSize;  i <= foldernameSize + 1; --i) {
        if (foldername[i] == '.') {
            nonExFoldername = foldername.erase(i, foldernameSize);
            break;
        }
    }

    filepath = nonExFoldername + ".fbm/" + filename;
}
void SkinMesh::Serialization(const char* fbxFileName, std::vector<Animation>* animations) {
    std::filesystem::path cerealFileName(fbxFileName);
    cerealFileName.replace_extension("cereal");

    std::ofstream ofs(cerealFileName.c_str(), std::ios::binary);
    cereal::BinaryOutputArchive serialization(ofs);
    serialization(sceneView, meshes, materials, *animations);
}

void SkinMesh::BlendAnimations(Animation::KeyFrame& output, const Animation::KeyFrame* start, const Animation::KeyFrame* end, float factor) {
    using namespace DirectX;

    size_t nodeCount = start->nodes.size();
    output.nodes.resize(nodeCount);
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
        XMVECTOR S[2] = {
            { start->nodes.at(nodeIndex).scaling.GetXM() },
            { end->nodes.at(nodeIndex).scaling.GetXM() }
        };
        XMStoreFloat3(&output.nodes.at(nodeIndex).scaling, XMVectorLerp(S[0], S[1], factor));

        XMVECTOR R[2] = {
            { start->nodes.at(nodeIndex).rotationQ.GetXM() },
            { end->nodes.at(nodeIndex).rotationQ.GetXM() }
        };
        XMStoreFloat4(&output.nodes.at(nodeIndex).rotationQ, XMQuaternionSlerp(R[0], R[1], factor));

        XMVECTOR T[2] = {
            { start->nodes.at(nodeIndex).translation.GetXM() },
            { end->nodes.at(nodeIndex).translation.GetXM() }
        };
        XMStoreFloat3(&output.nodes.at(nodeIndex).translation, XMVectorLerp(T[0], T[1], factor));
    }
}
void SkinMesh::OnlySomeAnimation(Animation::KeyFrame& out, const Animation::KeyFrame& in, int* animationIndices, size_t indicesSize) {
    out.nodes.resize(in.nodes.size());
    for (size_t i = 0; i < indicesSize; ++i) {
        int index = animationIndices[i];
        out.nodes[index] = in.nodes[index];
    }
}
void SkinMesh::OnlysomeBlendAnimation(Animation::KeyFrame& output, const Animation::KeyFrame* start, const Animation::KeyFrame* end, float factor, int* animationIndices, size_t indicesSize) {
    using namespace DirectX;

    size_t nodeCount = start->nodes.size();
    output.nodes.resize(nodeCount);

    for (size_t animationIndex = 0; animationIndex < indicesSize; ++animationIndex) {
        int nodeIndex = animationIndices[animationIndex];

        XMVECTOR S[2] = {
            { start->nodes.at(nodeIndex).scaling.GetXM() },
            { end->nodes.at(nodeIndex).scaling.GetXM() }
        };
        XMStoreFloat3(&output.nodes.at(nodeIndex).scaling, XMVectorLerp(S[0], S[1], factor));

        XMVECTOR R[2] = {
            { start->nodes.at(nodeIndex).rotationQ.GetXM() },
            { end->nodes.at(nodeIndex).rotationQ.GetXM() }
        };
        XMStoreFloat4(&output.nodes.at(nodeIndex).rotationQ, XMQuaternionSlerp(R[0], R[1], factor));

        XMVECTOR T[2] = {
            { start->nodes.at(nodeIndex).translation.GetXM() },
            { end->nodes.at(nodeIndex).translation.GetXM() }
        };
        XMStoreFloat3(&output.nodes.at(nodeIndex).translation, XMVectorLerp(T[0], T[1], factor));
    }
}

void SkinMesh::Render(RenderSystem* renderSystem, const Matrix& world, const Vec4& materialColor, const Animation::KeyFrame* keyframe) {
    ID3D11DeviceContext* immediateContext = renderSystem->immediateContext.Get();
    renderSystem->SwitchBlendState(blendType);
    renderSystem->SwitchDepthStencil(depthState);
    renderSystem->SwitchRasterizer(rasterizerState);


    for (const Mesh& mesh : meshes) {
        uint32_t stride = sizeof(Vertex);
        uint32_t offset = 0;
        immediateContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
        immediateContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        immediateContext->IASetInputLayout(inputLayout.Get());

        immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
        immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

        using namespace DirectX;
        // about animation
        Constants data;
        // if keyframe is availd
        if (keyframe && keyframe->nodes.size() > 0) {
            // get anime node (mesh need).  and sceme world transform is [it * argument transform]
            const Animation::KeyFrame::Node& meshNode = keyframe->nodes.at(mesh.nodeIndex);
            XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&meshNode.globalTransform) * world.GetXM());

            const size_t boneCount = mesh.bindPose.bones.size();
            _ASSERT_EXPR(boneCount < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

            for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex) {
                const Skelton::Bone& bone = mesh.bindPose.bones.at(boneIndex);
                const Animation::KeyFrame::Node& boneNode = keyframe->nodes.at(bone.nodeIndex);
                XMStoreFloat4x4(&data.boneTransform[boneIndex],
                    bone.offsetTransform.GetXM() *
                    boneNode.globalTransform.GetXM() *
                    XMMatrixInverse(nullptr, mesh.defaultGlobalTransform.GetXM()));
            }
        }
        else {
            XMStoreFloat4x4(&data.world,
                mesh.defaultGlobalTransform.GetXM() * world.GetXM());
            for (size_t boneIndex = 0; boneIndex < MAX_BONES; ++boneIndex) {
                data.boneTransform[boneIndex] = Matrix();
            }
        }
        for (const Mesh::Subset& subset : mesh.subsets) {
            auto it = materials.find(subset.materialUniqueID);

            if (it != materials.end()) {

                const Material& material = materials.at(subset.materialUniqueID);
                data.materialColor = materialColor * material.Kd;
                data.ambientLight = material.Ka + forceAmbientColor;

                immediateContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
                immediateContext->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());
            }
            else {
                data.materialColor = materialColor;
                data.ambientLight = Vec4(1.0f);
            }
            immediateContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
            immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
            immediateContext->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

            immediateContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
        }
    }
}

void SkinMeshManager::LoadMesh(std::vector<Animation>& animations, const char* fbxFileName, float samplingRate, bool triangulate,
    COORDINATE_TYPE coordinateType, Vec4 forceAmbientColor,
    RASTERIZER_STATE rs, BLEND_TYPE bt, DEPTH_STATE ds, bool serialize)
{
    meshes.push_back(SkinMesh(&animations, renderSystem->device.Get(), fbxFileName, samplingRate, triangulate, coordinateType, rs, bt, ds, forceAmbientColor, serialize));
}

// Mesh and animation merge in [ render ] function, so delete it;
// void SkinMeshManager::UpdateAnimation(int index, Animation::KeyFrame& keyframe)
// {
//     meshes.at(index).UpdateAnimation(keyframe);
// }
void SkinMeshManager::Render(int index, const Animation::KeyFrame* keyframe, const Matrix& worldMatrixLeftYup, const Vec4& materialColor)
{
    meshes.at(index).Render(renderSystem, worldMatrixLeftYup, materialColor, keyframe);
}