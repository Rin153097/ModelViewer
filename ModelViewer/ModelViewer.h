#pragma once

#include "../CCL/SkinnedMesh.h"
#include "../CCL/PrimitiveModel.h"
#include "../CCL/Primitive2D.h"

class ModelViewer {
public:
	ModelViewer(RenderSystem* renderSystem);

private:
	RenderSystem*				renderSystem;
private: // primitiveRenderers
	PrimitiveModel primitives;
	PrimitiveRect  primitiveRect;

private: // modelData
	std::unique_ptr<SkinMesh>	model;
	AnimationManager			animations;

	Transform					transform;
	const char*					fbxFilename;
	int							meshIndex = 0;

	bool						openFileControl;
	bool						warning;

private:
	enum class ViewerMode {
		BlendAnimation,
		AnimationManagerEditor,
		MaterialEditor,
	} mode = ViewerMode::BlendAnimation;

private:
	class ModeBlendAnimation {
	public:
		ModeBlendAnimation(ModelViewer* modelViewer) : modelViewer(modelViewer) {}

	private:
		ModelViewer* modelViewer;

	private: // select Range
		Vec2 selectS = {};
		Vec2 selectE = {};
		const float resetTime = 0.5f;
		float resetTimer = 0.0f;
		bool resetRemind = false;
		bool selectActive = false;

	private: // MeshAnimationData
		Animation::KeyFrame	 blendKeyframe;
		Animation::KeyFrame	 tmpKeyframe;
	public:
		Animation::KeyFrame* keyP = nullptr;

	private: // ImGui mode
		bool bonesListMode = false;
		bool rasterizerState = true;

	private: // MeshNodeData

		struct KeyframeData {
			std::vector<int>		blendIndex;

			Animation::KeyFrame* keyframe = nullptr;

			int animeIndex = 0;
			int sequence = 0;
			float blendFactor = 0.0f;  // blendAnimation( animes[animeIndices[0]],animes[animeIndices[1]], animeIndices[1].blendFactor)
			bool play = false;
			bool loop = true;
			bool autoBlendLoop = true;

			int sequenceStart = 0;
			int sequenceEnd = 0;

			float animationTime = 0.0f;

			float autoBlend = 0.0f;
		};
		std::vector<KeyframeData> keyframeData;

		std::vector<int> remind;		// if push bone button, remind it animationIndex.
		std::vector<int> tmpRemind;

	public:
		void Update(float elapsedTime) {
			UpdateSelectRange();
			UpdateAnimation(elapsedTime);
		}
		void Render(float screenWidth, float screenHeight, const Matrix& projection, const Matrix& view) {
			RenderModel();
			RenderRemind();
			RenderAndUpdateSelectBone(screenWidth, screenHeight, projection, view);

			RenderSelectRect();
		}
	private:
		void UpdateSelectRange();

		void UpdateAnimation(float elapsedTime);
		void UpdateRasterizer();
		void UpdateAnimationsManager();
		void UpdateAnimationPlay();
		void UpdateKeyframe(float elapsedTime);
		void BlendKeyframes(float elapsedTime);

		void RenderRemind();
		void RenderAndUpdateSelectBone(float screenWidth, float screenHeight, const Matrix& projection, const Matrix& view);
		void RenderSelectRect();
		void RenderModel();


	private:
		void UpdateKeyframeData(float elapsedTime, KeyframeData& animeIndex, size_t index);
		void RenderBonePoint(Vec2 pos, int index);

		void AddRemind(std::vector<int>& remind, int value);
		void AppendBlendIndex(std::vector<int>& out, std::vector<int>& in) { out = in; }
		void AddAnimation() { keyframeData.emplace_back(); }
		void ClearAnimation() { keyframeData.clear(); }

		bool FindFromRemind(int value) {
			for (int i : remind) {
				if (i == value) return true;
			}
			return false;
		}
		bool CheckHitBonePointVsSelectRange(Vec2 pos);
	} modeBlendAnimation;

	class ModeAnimationEditor {
	public:
		ModeAnimationEditor(ModelViewer* modelViewer) : modelViewer(modelViewer) {}
	private:
		enum class EditorMode {
			PlayAnimation,
			Delete,
			NameEditor
		} editMode;
		std::string modename = "AnimationView";
		class NameEditor {
		public:
			NameEditor() {}
			static const size_t nameSizeMax = 255;
			std::string newName = {};
		} nameEditor;

	private:
		ModelViewer*	modelViewer;
		int				animeIndex = 0;

		bool loop = true;
		bool play = false;
		bool reNaming = false;

		int		animeSequence	= 0;
		float	animationTime	= 0.0f;
		float	playMag			= 1.0f;	// magnification


	public:
		void Update(float elapsedTime) {
			UpadateAnimationList(elapsedTime);
		}
		void Render() {
			RenderModel();
		}

		void UpadateAnimationList(float elapsedTime);
		void RenderModel();

		void UpdatePlayAnimation(size_t index);
		void UpdateDelete(size_t index);
		void UpdateNameEditor(size_t index);

	} modeAnimationEditor;

	class MaterialEditor {
	public:
		MaterialEditor(ModelViewer* modelViewer) : modelViewer(modelViewer) {}

	private:
		ModelViewer* modelViewer;
		bool reNaming = false;
	private:
		class PathEditor {
		public:
			PathEditor() {}
			static const size_t nameSizeMax = 255;
			std::string newName = {};
			size_t renameMaterialKey;
			int renameFilePathIndex;
		} pathEditor;

	public:
		void Update();
		void Render();

	private:
		void UpdateRenameFile();
		void UpdateMaterialList();

	} modeMaterialEditor;

private:
	void UpdateViewMode();
	void UpdateTransform();

	void UpdateNullModelAssert();

	void LoadMeshAndAnimeData();
	void LoadMeshData();
	void LoadAnimeData();
	void Serialize();

	void RenderModel();
	void RenderViewerWorld();

public:
    void Init();
    void Update(float elapsedTime);
    void Render(float screenWidth, float screenHeight, const Matrix& projection, const Matrix& view);
};