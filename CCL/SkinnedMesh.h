#pragma once

#include "Vector.h"

#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>
#include <set>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

#include "RenderSystem.h"

namespace DirectX {
    template<class T>
    void serialize(T& archive, Vec2& v) {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class T>
    void serialize(T& archive, Vec3& v) {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class T>
    void serialize(T& archive, Vec4& v) {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class T>
    void serialize(T& archive, Matrix& m) {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

enum class COORDINATE_TYPE {
    RIGHT_YUP = 0,
    LEFT_YUP,
    RIGHT_ZUP,
    LEFT_ZUP,
};

struct Scene {
    struct Node {
        uint64_t uniqueID = 0;
        std::string name;
        FbxNodeAttribute::EType attribute = FbxNodeAttribute::EType::eUnknown;
        int64_t parentIndex = -1;

        template <class T>
        void serialize(T& archive) {
            archive(uniqueID, name, attribute, parentIndex);
        }
    };
    std::vector<Node> nodes;
    int64_t indexof(uint64_t uniqueID) const {
        int64_t index = 0;
        for (const Node& node : nodes) {
            if (node.uniqueID == uniqueID) {
                return index;
            }
            ++index;
        }
        return -1;
    }
    template <class T>
    void serialize(T& archive) {
        archive(nodes);
    }
};
struct Skelton {
    struct Bone {
        uint64_t  uniqueID = 0;
        std::string name;
        int64_t parentIndex = -1;
        int64_t nodeIndex = 0;

        Matrix offsetTransform = Matrix();

        bool isOrphan() const { return parentIndex < 0; }

        template<class T>
        void serialize(T& archive) {
            archive(uniqueID, name, parentIndex, nodeIndex, offsetTransform);
        }
    };
    std::vector<Bone> bones;
    int64_t indexof(uint64_t uniqueID) const {
        int64_t index = 0;
        for (const Bone& bone : bones) {
            if (bone.uniqueID == uniqueID) {
                return index;
            }
            ++index;
        }
        return -1;
    }
    template <class T>
    void serialize(T& archive) {
        archive(bones);
    }
};
struct Animation {
    std::string name;
    float samplingRate = 0;

    struct KeyFrame {
        struct Node {
            Matrix globalTransform = Matrix();

            Vec3 scaling = { 1.0f,1.0f,1.0f };
            Vec4 rotationQ = { 0,0,0,1 };
            Vec3 translation = { 0,0,0 };

            template<class T>
            void serialize(T& archive) {
                archive(globalTransform, scaling, rotationQ, translation);
            }
        };
        std::vector<Node> nodes;
        template <class T>
        void serialize(T& archive) {
            archive(nodes);
        }
    };
    std::vector<KeyFrame> sequence;
    template <class T>
    void serialize(T& archive) {
        archive(name, samplingRate, sequence);
    }
};

class AnimationManager {
public:
    std::vector<Animation> animations;

    size_t GetKeyFrameSize(int index) { return animations.at(index).sequence.size(); }
    float GetAnimationOneLoopTime(int index) { return static_cast<float>(GetKeyFrameSize(index)) / static_cast<float>(animations.at(index).samplingRate); }

    Animation::KeyFrame& GetStartFrame(int index) { return animations.at(index).sequence.at(0); }
    Animation::KeyFrame& GetEndFrame(int index) { return animations.at(index).sequence.at(GetKeyFrameSize(index) - 1); }

    // ToDo éûä‘Ç©ÇÁÉtÉåÅ[ÉÄÇéÊìæÅA
    Animation::KeyFrame& GEtKeyFrame(int index, float time);
};

class SkinMesh {
public:  // structs
    static const int MAX_BONE_INFLUENCES = 4;
    struct Vertex {
        Vec3 position;
        Vec3 normal;
        Vec4 tangent;
        Vec2 texCoord;
        float boneWeight[MAX_BONE_INFLUENCES] = { 1,0,0,0 };
        uint32_t boneIndices[MAX_BONE_INFLUENCES] = {};

        template <class T>
        void serialize(T& archive) {
            archive(position, normal, tangent, texCoord, boneWeight, boneIndices);
        }
    };
    static const int MAX_BONES = 256;
    struct Constants {
        Matrix world;
        Vec4 materialColor;
        Vec4 ambientLight;
        Matrix boneTransform[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
    };
    struct Mesh {
        Matrix defaultGlobalTransform = Matrix();
        Vec3 boundingBox[2] = {
            { +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
            { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
        };

        uint64_t uniqueID = {};
        std::string name;

        int64_t nodeIndex = {};


        Skelton bindPose;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

        struct Subset {
            uint64_t materialUniqueID = 0;
            std::string materialName;

            uint32_t startIndexLocation = 0;
            uint32_t indexCount = 0;

            template <class T>
            void serialize(T& archive) {
                archive(materialUniqueID, materialName, startIndexLocation, indexCount);
            }
        };
        std::vector<Subset> subsets;

        template <class T>
        void serialize(T& archive) {
            archive(uniqueID, name, nodeIndex, subsets, defaultGlobalTransform, bindPose, boundingBox, vertices, indices);
        }
    private:
        friend class skinnedMesh;
    };
    struct Material {
        uint64_t uniqueID = 0;
        std::string name;

        Vec4 Ka = { 0.2f, 0.2f, 0.2f, 1.0f };
        Vec4 Kd = { 0.8f, 0.8f, 0.8f, 1.0f };
        Vec4 Ks = { 1.0f, 1.0f, 1.0f, 1.0f };

        std::string textureFileName[4];
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[4];
        bool operator<(const Material& rhs) { return uniqueID < rhs.uniqueID; }

        template <class T>
        void serialize(T& archive) {
            archive(uniqueID, name, Ka, Kd, Ks, textureFileName);
        }
    };

public:
    BLEND_TYPE          blendType;
    DEPTH_STATE         depthState;
    RASTERIZER_STATE    rasterizerState;

    Vec4                forceAmbientColor;


public: // data
    std::vector<Mesh> meshes;
    std::unordered_map<uint64_t, Material> materials;

protected:
    Scene sceneView;

protected: // com Object
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        constantBuffer;

public: // make data
    void FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes);
    void FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);
    void FetchSkelton(FbxMesh* fbxMesh, Skelton& bindPose);
    void CreateComObjects(ID3D11Device* device, const char* fbxFileName);

    void FetchAnimation(FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate);
    bool AppendAnimations(std::vector<Animation>& animations, const char* animationFileName, float samplingRate);

    void FixFileName(std::string& filePath, std::string& fbxFilename);

    void Serialization(const char* fbxFileName, std::vector<Animation>* animations);

public: // update

    static void BlendAnimations(Animation::KeyFrame& output, const Animation::KeyFrame* start, const Animation::KeyFrame* end, float factor);
    static void OnlySomeAnimation(Animation::KeyFrame& out, const Animation::KeyFrame& in, int* animationIndices, size_t indicesSize);
    static void OnlysomeBlendAnimation(Animation::KeyFrame& output, const Animation::KeyFrame* start, const Animation::KeyFrame* end, float factor, int* animationIndices, size_t indicesSize);
    static void UpdateAnimation(Animation::KeyFrame& keyframe, const SkinMesh& mesh);

    void Render(RenderSystem* renderSystem, const Matrix& world = Matrix(), const Vec4& materialColor = Vec4(1.0f), const Animation::KeyFrame* keyframe = nullptr);

public: // constractors
    SkinMesh(std::vector<Animation>* animations, ID3D11Device* device, const char* fbxFileName, float samplingRate = 0.0f, bool triangulate = false,
        COORDINATE_TYPE coordinateType = COORDINATE_TYPE::LEFT_YUP,
        RASTERIZER_STATE rs = RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE bt = BLEND_TYPE::ALPHA, DEPTH_STATE ds = DEPTH_STATE::ENABLE_AND_WRITE,
        Vec4 forceAmbientColor = Vec4(), bool serialize = true
    );
    virtual ~SkinMesh() = default;
};

class SkinMeshManager {
private:
    RenderSystem* renderSystem;
    std::vector<SkinMesh>   meshes;

public:
    SkinMeshManager(RenderSystem* renderSystem = nullptr) : renderSystem(renderSystem) {}
    void SetRenderSystem(RenderSystem* renderSystem) { this->renderSystem = renderSystem; }

public:
    void LoadMesh(std::vector<Animation>& animations, const char* fbxFileName, float samplingRate = 0.0f, bool triangulate = false,
        COORDINATE_TYPE coordinateType = COORDINATE_TYPE::LEFT_YUP, Vec4 forceAmbientColor = Vec4(),
        RASTERIZER_STATE rs = RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE bt = BLEND_TYPE::ALPHA, DEPTH_STATE ds = DEPTH_STATE::ENABLE_AND_WRITE, bool serialize = true);

    //  void UpdateAnimation(int index, Animation::KeyFrame& keyframe);
    void Render(int index, const Animation::KeyFrame* keyframe, const Matrix& worldMatrixLeftYup, const Vec4& materialColor);

    SkinMesh& Get(int index) { return meshes.at(index); }
};