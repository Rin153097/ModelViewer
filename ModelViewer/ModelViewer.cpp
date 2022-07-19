#include "../CCL/CCL.h"

#include "ModelViewer.h"
#include "../CCL/InputPrimitive.h"

#include "../CCL/TextManager.h"

#include "../CCL/Template/inlines.h"

#include "../CCL/Template/OpenFileDialog.h"

#define FORCE_LOADING 0

ModelViewer::ModelViewer(RenderSystem* renderSystem)
	: primitives(renderSystem), primitiveRect(renderSystem),
	meshIndex(0), renderSystem(renderSystem), model(), animations(),
#if FORCE_LOADING
	fbxFilename("./resources/PlayerFlap/flap.fbx"),
#else
	fbxFilename(""),
#endif
	// fbxFilename("./Assets/unitychan.fbx"),
	modeBlendAnimation(this),
	modeAnimationEditor(this),
	modeMaterialEditor(this),
	openFileControl(false)
{
#if FORCE_LOADING
	model = std::make_unique<SkinMesh>(&animations.animations, renderSystem->device.Get(),
		fbxFilename,
		0.0f, true,
		// COORDINATE_TYPE::RIGHT_YUP, RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, Vec4(0.5f), true);
		COORDINATE_TYPE::RIGHT_YUP, RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, Vec4(0.5f), false);
#endif

	// model->AppendAnimations(animations.animations, "./resources/PlayerFlap/Idle.fbx", 0.0f);
	// model->AppendAnimations(animations.animations, "./resources/PlayerFlap/Kneeling Down.fbx", 0.0f);
	// model->AppendAnimations(animations.animations, "./resources/PlayerFlap/Hip Hop Dancing.fbx", 0.0f);
}


/*********************************************************************/
/***********************/  /*  System  */  /***************************/
/*********************************************************************/

void ModelViewer::Init() {
}
void ModelViewer::Update(float elapsedTime) {

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(1280, 36), ImGuiSetCond_Always);
	ImGui::Begin("MENU", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);
	std::string modeName;
	switch (mode) {
	case ViewerMode::BlendAnimation:			modeName = "BlendAnimationMode : ";			break;
	case ViewerMode::AnimationManagerEditor:    modeName = "AnimationManagerEditor : ";		break;
	case ViewerMode::MaterialEditor:			modeName = "MaterialEditor : ";				break;
	}

	ImGui::Text(modeName.c_str()); ImGui::SameLine();
	if (ImGui::Button("File")) { openFileControl = true; } ImGui::SameLine();
	if (ImGui::Button("BlendAnimation"))	{ mode = ViewerMode::BlendAnimation; } ImGui::SameLine();
	if (ImGui::Button("AnimeListEditor"))	{ mode = ViewerMode::AnimationManagerEditor; } ImGui::SameLine();
	if (ImGui::Button("MaterialEditor"))	{ mode = ViewerMode::MaterialEditor; }
	
	ImGui::End();

	if (openFileControl) {
		if (ImGui::Button("LoadModelAndFbxData"))	{ LoadMeshAndAnimeData(); openFileControl = false; } ImGui::SameLine();
		if (ImGui::Button("LoadModelData"))			{ LoadMeshData(); openFileControl = false;} ImGui::SameLine();
		if (ImGui::Button("LoadAnimeData"))			{ LoadAnimeData(); openFileControl = false; } ImGui::SameLine();
		if (ImGui::Button("Serialize"))				{ Serialize(); openFileControl = false; } ImGui::SameLine();

		if (ImGui::Button("Close")) { openFileControl = false; } 
	}

	UpdateNullModelAssert();
	if (model.get() == nullptr) return;

	UpdateTransform();

	switch (mode) {
	case ViewerMode::BlendAnimation:			modeBlendAnimation.Update(elapsedTime); break;
	case ViewerMode::AnimationManagerEditor:	modeAnimationEditor.Update(elapsedTime); break;
	case ViewerMode::MaterialEditor:			modeMaterialEditor.Update(); break;
	}
}
void ModelViewer::Render(float screenWidth, float screenHeight, const Matrix& projection, const Matrix& view) {
	RenderViewerWorld();
	if (model.get() == nullptr) return;

	switch (mode) {
	case ViewerMode::BlendAnimation:			modeBlendAnimation.Render(screenWidth, screenHeight, projection, view); break;
	case ViewerMode::AnimationManagerEditor:	modeAnimationEditor.Render(); break;
	case ViewerMode::MaterialEditor:			modeMaterialEditor.Render(); break;
	}
}
void ModelViewer::RenderModel() {
	Vec4 color = Vec4(1.0f);
	model->Render(renderSystem, transform.transform, color, modeBlendAnimation.keyP);
}
void ModelViewer::RenderViewerWorld() {
	primitives.RenderGrid(Matrix(0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	primitives.RenderTransform(Matrix(0.004f, 0.0f, 0.0f, 0.0f, 0.0f, 0.004f, 0.0f, 0.0f, 0.0f, 0.0f, 0.004f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
}

void ModelViewer::UpdateTransform() {
	ImGui::Begin("Transform");
	ImGui::InputFloat3("rotation", &transform.rotation.pitchYawRoll.x);
	ImGui::InputFloat3("scale", &transform.scale.x);
	ImGui::InputFloat3("translation", &transform.translation.x);

	static float allScale = 1.0f;
	if (ImGui::InputFloat("AllScale", &allScale)) {
		transform.scale = allScale;
	}
	ImGui::End();

	transform.UpdateTransformRPY();
}

// About LoadData
void ModelViewer::UpdateNullModelAssert() {
	if (warning) {
		ImVec2 wh = { 500.0f, 95.0f };
		ImVec2 pos = { 1280.0f / 2.0f - wh.x / 2.0f, 720.f / 2.0f - wh.y / 2.0f };

		ImGui::SetNextWindowPos(pos, ImGuiCond_::ImGuiCond_Always);
		ImGui::SetNextWindowSize(wh, ImGuiCond_::ImGuiCond_Always);

		ImGui::Begin("Warning");
		ImGui::Text("ModelData is not loading. ");
		ImGui::Text("if You want to load Animation, you must load modelData first.");
		ImGui::Text("                              "); ImGui::SameLine();
		if (ImGui::Button("OK")) { warning = false; }
		ImGui::End();
	}
}
void ModelViewer::LoadMeshAndAnimeData() {
	FilePathManager::GetPathByDialog();
	size_t size = FilePathManager::GetSize();
	for (size_t i = 0; i < size; ++i) {
		const ATL::CStringA& path = FilePathManager::GetPath();

		fbxFilename = path.GetString();
		model = std::make_unique<SkinMesh>(&animations.animations, renderSystem->device.Get(),
			fbxFilename, 0.0f, true,
			COORDINATE_TYPE::RIGHT_YUP, RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, Vec4(0.5f), false);
	}
}
void ModelViewer::LoadMeshData() {
	FilePathManager::GetPathByDialog();
	size_t size = FilePathManager::GetSize();
	for (size_t i = 0; i < size; ++i) {
		const ATL::CStringA& path = FilePathManager::GetPath();

		fbxFilename = path.GetString();
		model = std::make_unique<SkinMesh>(nullptr, renderSystem->device.Get(),
			fbxFilename, 0.0f, true,
			COORDINATE_TYPE::RIGHT_YUP, RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, Vec4(0.5f), false);
	}
}
void ModelViewer::LoadAnimeData() {
	if (model.get() == nullptr) {
		warning = true;
		return;
	}

	FilePathManager::GetPathByDialog();
	size_t size = FilePathManager::GetSize();
	for (size_t i = 0; i < size; ++i) {
		const ATL::CStringA& path = FilePathManager::GetPath();

		model->AppendAnimations(animations.animations, path.GetString(), 0.0f);
	}
}
void ModelViewer::Serialize() {
	if (model.get() == nullptr) {
		warning = true;
		return;
	}
	model->Serialization(fbxFilename, &animations.animations);
}

/*********************************************************************/
/***********************/  /*  Blend  */  /***************************/
/*********************************************************************/

void ModelViewer::ModeBlendAnimation::UpdateAnimation(float elapsedTime) {
	ImGui::Begin("MeshData");

	UpdateRasterizer();
	UpdateAnimationsManager();
	UpdateAnimationPlay();

	UpdateKeyframe(elapsedTime);
	
	ImGui::End();
}

void ModelViewer::ModeBlendAnimation::UpdateRasterizer() {
	const char* rsText = rasterizerState ? "Fill" : "Wire";
	if (ImGui::Button(rsText)) { rasterizerState = !rasterizerState; }
	modelViewer->model->rasterizerState = rasterizerState ? RASTERIZER_STATE::FILL_AND_CULL : RASTERIZER_STATE::WIREFRAME_NOT_CULL;
}
void ModelViewer::ModeBlendAnimation::UpdateAnimationsManager() {
	if (ImGui::Button("AddAnimation")) {
		AddAnimation();
	} ImGui::SameLine();
	if (ImGui::Button("ClearAnimation")) {
		ClearAnimation();
	}
}
void ModelViewer::ModeBlendAnimation::UpdateAnimationPlay() {
	if (ImGui::Button("PlayAll")) {
		for (KeyframeData& kfd : keyframeData) {
			kfd.play = true;
			kfd.animationTime = 0.0f;
		}
	} ImGui::SameLine();
	if (ImGui::Button("StopAll")) {
		for (KeyframeData& kfd : keyframeData) {
			kfd.play = false;
			kfd.animationTime = 0.0f;
		}
	}
}
void ModelViewer::ModeBlendAnimation::UpdateKeyframe(float elapsedTime) {
	if (modelViewer->animations.animations.size()) {
		// decide anime sequence
		if (keyframeData.size()) {
			BlendKeyframes(elapsedTime);
			keyP = &blendKeyframe;
		}
		else {
			keyP = nullptr;
		}
	}
}
void ModelViewer::ModeBlendAnimation::BlendKeyframes(float elapsedTime) {

	size_t size = keyframeData.size();
	for (size_t i = 0; i < size; ++i) {
		UpdateKeyframeData(elapsedTime, keyframeData[i], i);
	}

	// blend Init
	if (keyframeData.at(0).keyframe) {
		tmpKeyframe = *keyframeData.at(0).keyframe;
		blendKeyframe = *keyframeData.at(0).keyframe;
	}
	// blendAnimation
	for (size_t i = 1; i < size; ++i) {
		// onkysomeAnimation
		if (keyframeData.at(i).blendIndex.size()) {
			Animation::KeyFrame tmp;
			SkinMesh::OnlySomeAnimation(tmp,
				*keyframeData.at(i).keyframe,
				keyframeData.at(i).blendIndex.data(),
				keyframeData.at(i).blendIndex.size()
			);
			SkinMesh::OnlysomeBlendAnimation(
				blendKeyframe,
				&blendKeyframe,
				&tmp,
				keyframeData[i].blendFactor,
				keyframeData[i].blendIndex.data(),
				keyframeData[i].blendIndex.size()
			);
		}
		else {
			SkinMesh::BlendAnimations(
				blendKeyframe,
				&blendKeyframe,
				keyframeData.at(i).keyframe,
				keyframeData[i].blendFactor
			);
		}
	}

	SkinMesh::UpdateAnimation(blendKeyframe, *modelViewer->model.get());
}

void ModelViewer::ModeBlendAnimation::UpdateSelectRange() {
	selectActive = false;
	if (KeyboardManager::Instance().Release(CCL_K_LSTICK)) {
		tmpRemind = remind;
	}
	// if click ALT. user is controlling camera. 
	if (KeyboardManager::Instance().State(CCL_K_LTRG1)) return;
	// if click L
	if (KeyboardManager::Instance().State(CCL_K_LSTICK)) {
		selectActive = true;
		if (KeyboardManager::Instance().Trg(CCL_K_LSTICK)) {
			// if first frame, remind cursor position.
			selectS = Cursor::GetCursorClientPos();
		}
		// anytime, .....
		selectE = Cursor::GetCursorClientPos();	
	}
}

void ModelViewer::ModeBlendAnimation::UpdateKeyframeData(float elapsedTime, KeyframeData& keyframeData, size_t index) {
	
	std::string treeNodeName = std::to_string(index) + " : " + (modelViewer->animations.animations.at(keyframeData.animeIndex).name.size() ?
		modelViewer->animations.animations.at(keyframeData.animeIndex).name.c_str() : "null");
	
	// make tree Node
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_::ImGuiCond_Once);
	if (ImGui::TreeNode(treeNodeName.c_str())) {

		// about AnimeSequence
		if (ImGui::InputInt("AnimeIndex", &keyframeData.animeIndex, 1)) {
			keyframeData.animeIndex = clamp(keyframeData.animeIndex, 0, static_cast<int>(modelViewer->animations.animations.size() - 1));
			keyframeData.animationTime = 0.0f;
			keyframeData.sequenceStart = 0;
			keyframeData.sequenceEnd = static_cast<int>(modelViewer->animations.animations.at(keyframeData.animeIndex).sequence.size()) - 1;
			keyframeData.sequence = 0;
		}

		// about playAnimation Auto
		const char* playOrStop = !keyframeData.play ? "Play" : "Stop";
		if (ImGui::Button(playOrStop)) {
			if (keyframeData.play) {
				keyframeData.play = false;
				keyframeData.animationTime = 0.0f;
			} 
			else {
				keyframeData.play = true;
			}
		} ImGui::SameLine();
		ImGui::Checkbox("Loop", &keyframeData.loop); ImGui::SameLine();

		// blend factor
		ImGui::Checkbox("AutoBlendLoop", &keyframeData.autoBlendLoop);
		ImGui::SliderFloat("autoBlend", &keyframeData.autoBlend, 0.0f, 10.0f);

		ImGui::SliderFloat("BlendFactor", &keyframeData.blendFactor, 0.0f, 1.0f);
		keyframeData.blendFactor += keyframeData.autoBlend * elapsedTime;
		if (keyframeData.blendFactor > 1.0f) {
			if (keyframeData.autoBlendLoop) {
				keyframeData.blendFactor -= 1.0f;
			}
			else {
				keyframeData.blendFactor = 1.0f;
			}
		}
		

		if (ImGui::SliderInt2("SequenceClamp", &keyframeData.sequenceStart, 0, static_cast<int>(modelViewer->animations.animations.at(keyframeData.animeIndex).sequence.size() - 1))) {
			keyframeData.animationTime = 0.0f;
			keyframeData.sequence = keyframeData.sequenceStart;
		}

		// decide anime sequence
		if (keyframeData.play) {
			keyframeData.sequence = static_cast<int>(keyframeData.animationTime * modelViewer->animations.animations.at(keyframeData.animeIndex).samplingRate);
			keyframeData.sequence += keyframeData.sequenceStart;

			// if index is over the index max
			if (keyframeData.sequence > keyframeData.sequenceEnd) {

				if (keyframeData.loop) {
					// sequence back to start.
					keyframeData.sequence = keyframeData.sequenceStart;
					keyframeData.animationTime = 0.0f;
				}
				else {
					keyframeData.sequence = keyframeData.sequenceEnd;
					keyframeData.play = false;
				}
			}
			else {
				keyframeData.animationTime += elapsedTime;
			}
		}
		else {
			// if not playing animation, user can control sequence.
			Animation& anime = modelViewer->animations.animations.at(keyframeData.animeIndex);
			if (anime.sequence.size()) {
				ImGui::SliderInt("animeSequence", &keyframeData.sequence, 0, static_cast<int>(anime.sequence.size() - 1));
			}
		}
		keyframeData.keyframe = &modelViewer->animations.animations.at(keyframeData.animeIndex).sequence.at(keyframeData.sequence);

		ImGui::TreePop();
	}
}

void ModelViewer::ModeBlendAnimation::RenderModel() {
	Vec4 color = Vec4(1.0f);
	modelViewer->model->Render(modelViewer->renderSystem, modelViewer->transform.transform, color, keyP);
}
void ModelViewer::ModeBlendAnimation::RenderRemind() {
	ImGui::Begin("IndexArray");

	// copy remind Data in clipBox
	if (ImGui::Button("Copy")) {
		std::string cpytxt = "";

		// add "x"  (x is number).
		size_t size = remind.size();
		if (remind.size()) {
			std::string plus = std::to_string(remind.at(0));
			cpytxt += plus;
		}

		// add ", x".
		for (size_t i = 1; i < size; ++i) {
			std::string plus = ", " + std::to_string(remind.at(i));
			cpytxt += plus;
		}

		SetClipBoard(cpytxt.c_str());
	}
	resetRemind = false;
	if (ImGui::Button("Reset")) {
		resetRemind = true;
	}

	// append
	static int keyframeIndex = 0;
	if (keyframeData.size()) {
		ImGui::InputInt("appendIndex", &keyframeIndex);
		keyframeIndex = clamp(keyframeIndex, 0, static_cast<int>(keyframeData.size()) - 1);

		if (ImGui::Button("Append")) {
			AppendBlendIndex(keyframeData.at(keyframeIndex).blendIndex, remind);
		}
	}
	// output remind Data
	size_t remindSize = remind.size();
	for (size_t i = 0; i < remindSize; ++i) {
		ImGui::Text(std::to_string(remind.at(i)).c_str());
	}

	ImGui::End(); // IndexArray
}

void ModelViewer::ModeBlendAnimation::AddRemind(std::vector<int>& remind, int value) {
	size_t size = remind.size();
	for (size_t i = 0; i < size; ++i) {
		if (remind.at(i) == value) return;
	}
	remind.emplace_back(value);
}

void ModelViewer::ModeBlendAnimation::RenderAndUpdateSelectBone(float screenWidth, float screenHeight, const Matrix& projection, const Matrix& view) {
	// select range
	if (resetRemind) {
		tmpRemind.clear();
	}

	remind = tmpRemind;
	if (keyP) {
		size_t nodeSize = keyP->nodes.size();
		for (size_t i = 0; i < nodeSize; ++i) {
			Vec3 keyPos = keyP->nodes.at(i).globalTransform.GetTrans() * keyP->nodes.at(i).scaling * modelViewer->transform.scale;
			DirectX::XMVECTOR ScreenPos = DirectX::XMVector3Project(
				keyPos.GetXM(),
				0, 0,
				screenWidth, screenHeight,
				0.0f, 1.0f,
				projection.GetXM(),
				view.GetXM(),
				DirectX::XMMatrixIdentity()
			);
			Vec2 screenPos; DirectX::XMStoreFloat2(&screenPos, ScreenPos);

			// Check hit selectRange
			if (selectActive) {
				if (CheckHitBonePointVsSelectRange(screenPos))
				AddRemind(remind, static_cast<int>(i));
			}

			RenderBonePoint(screenPos,static_cast<int>(i));
		}
	}
	else {
		size_t nodeSize = modelViewer->model->meshes.at(modelViewer->meshIndex).bindPose.bones.size();
		for (size_t i = 0; i < nodeSize; ++i) {
			// Matrix defaultTrans = model->meshes.at(meshIndex).defaultGlobalTransform;
			// defaultTrans *= model->meshes.at(meshIndex).bindPose.bones.at(i).offsetTransform;
			Matrix debug = { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

			Matrix defaultTrans = debug * modelViewer->model->meshes.at(modelViewer->meshIndex).bindPose.bones.at(i).offsetTransform;
			defaultTrans *= modelViewer->model->meshes.at(modelViewer->meshIndex).defaultGlobalTransform;

			Vec3 nodePos = defaultTrans.GetTrans() * modelViewer->transform.scale;
			DirectX::XMVECTOR ScreenPos = DirectX::XMVector3Project(
				nodePos.GetXM(),
				0, 0,
				screenWidth, screenHeight,
				0.0f, 1.0f,
				projection.GetXM(),
				view.GetXM(),
				DirectX::XMMatrixIdentity()
			);
			Vec2 screenPos; DirectX::XMStoreFloat2(&screenPos, ScreenPos);

			if (selectActive) {
				if (CheckHitBonePointVsSelectRange(screenPos))
					AddRemind(remind, static_cast<int>(i));
			}

			RenderBonePoint(screenPos, static_cast<int>(i));
		}
	}
}

void ModelViewer::ModeBlendAnimation::RenderSelectRect() {
	if (selectActive)
		modelViewer->primitiveRect.Render(modelViewer->renderSystem,
			selectS.x, selectS.y, selectE.x - selectS.x, selectE.y - selectS.y,
			0.2f, 0.2f, 0.2f, 0.5f, 0.0f);
}

void ModelViewer::ModeBlendAnimation::RenderBonePoint(Vec2 pos, int index) {
	const Vec4 y = { 1.0f,0.0f,0.0f,1.0f };
	const Vec4 n = { 1.0f,1.0f,0.0f,1.0f };

	Vec4 color = FindFromRemind(index) ? y : n;

	modelViewer->primitiveRect.Render(modelViewer->renderSystem,
		pos.x, pos.y, 5.0f, 5.0f,
		color.x, color.y, color.z, color.w, 0.0f);
}

bool ModelViewer::ModeBlendAnimation::CheckHitBonePointVsSelectRange(Vec2 pos) {
	float minx = (std::min)(selectS.x, selectE.x);
	float miny = (std::min)(selectS.y, selectE.y);
	float maxx = (std::max)(selectS.x, selectE.x);
	float maxy = (std::max)(selectS.y, selectE.y);

	if (pos.x < minx) return false;
	if (pos.y < miny) return false;
	if (pos.x > maxx) return false;
	if (pos.y > maxy) return false;

	return true;
}



/*********************************************************************/
/**********************/  /*  Editor  */  /***************************/
/*********************************************************************/

void ModelViewer::ModeAnimationEditor::RenderModel() {
	if (modelViewer->animations.animations.size()) {
		Animation& anime = modelViewer->animations.animations.at(animeIndex);
		Animation::KeyFrame& keyframe = anime.sequence.at(animeSequence);
		modelViewer->model->Render(modelViewer->renderSystem, modelViewer->transform.transform, Vec4(1.0f), &keyframe);
	}
	else {
		modelViewer->model->Render(modelViewer->renderSystem, modelViewer->transform.transform, Vec4(1.0f), nullptr);
	}
}
void ModelViewer::ModeAnimationEditor::UpadateAnimationList(float elapsedTime) {
	if (reNaming) {
		ImVec2 wh = { 800.f, 100.0f };
		ImGui::SetNextWindowPos(ImVec2(1280 - wh.x , 720 - wh.y), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(wh.x, wh.y), ImGuiSetCond_Always);

		ImGui::Begin("Rename");
		if (ImGui::Button("OK")) {
			modelViewer->animations.animations.at(animeIndex).name = nameEditor.newName.c_str();
			reNaming = false;
			animeSequence = 0;
			animationTime = 0;
		} ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			reNaming = false;
			animeSequence = 0;
			animationTime = 0;
		}

		ImGui::InputText("new name", nameEditor.newName.data(), nameEditor.nameSizeMax);
		ImGui::End();

		Animation& anm = modelViewer->animations.animations.at(animeIndex);
		animeSequence = static_cast<int>(animationTime * anm.samplingRate);

		animationTime += elapsedTime;
		if (animeSequence > static_cast<int>(anm.sequence.size()) - 1) {
			animeSequence = 0;
			animationTime = 0.0f;
		}

		return;
	}

	ImGui::Begin("Mode");
	if (ImGui::Button("PlayAnimation")) { editMode = EditorMode::PlayAnimation; modename = "AnimationView"; }		ImGui::SameLine();
	if (ImGui::Button("Delete"))		{ editMode = EditorMode::Delete; modename = "It is Danger!! DeleteMode"; }	ImGui::SameLine();
	if (ImGui::Button("NameEdit"))		{ editMode = EditorMode::NameEditor; modename = "NameEditMode"; }			
	ImGui::End();

	bool changeStyle = false;
	if (editMode == EditorMode::Delete) {
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TitleBgActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TitleBg, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, ImVec4(1.0f, 0.8f, 0.8f, 1.0f));
	}
	ImGui::Begin("List");
	ImGui::Text(modename.c_str());

	for (size_t i = 0; i < modelViewer->animations.animations.size(); ++i) {
		ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_Once);
		if (ImGui::TreeNode(std::string("AnimationData : " + std::to_string(i)).c_str())) {
			Animation& anm = modelViewer->animations.animations.at(i);
			const char* anmName = anm.name.size() ? anm.name.c_str() : "null";
			if (ImGui::Button(anmName)) {
				switch (editMode) {
				case EditorMode::PlayAnimation: UpdatePlayAnimation(i); break;
				case EditorMode::Delete:		UpdateDelete(i);		break;
				case EditorMode::NameEditor:	UpdateNameEditor(i);	break;
				}
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();

	if (editMode == EditorMode::Delete) {
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	ImGui::Begin("Animation");
	if (modelViewer->animations.animations.size()) {
		if (ImGui::InputInt("AnimeIndex", &animeIndex)) {

		}
		animeIndex = clamp(animeIndex, 0, static_cast<int>(modelViewer->animations.animations.size()) - 1);

		Animation& anm = modelViewer->animations.animations.at(animeIndex);
		const char* playOrStop = play ? "Stop" : "Play";
		if (ImGui::Button(playOrStop)) { play = !play; }
		ImGui::Checkbox("Loop", &loop);
		ImGui::InputFloat("PlayMagnification", &playMag);

		if (play) {
			animeSequence = static_cast<int>(animationTime * anm.samplingRate);

			animationTime += elapsedTime * playMag;
			if (animeSequence > static_cast<int>(anm.sequence.size()) - 1) {
				animeSequence = 0;
				animationTime = 0.0f;
			}
		}
		else {
			ImGui::SliderInt("Sequence", &animeSequence, 0, static_cast<int>(anm.sequence.size()) - 1);
		}
	}
	ImGui::End();
}
void ModelViewer::ModeAnimationEditor::UpdatePlayAnimation(size_t index) {
	animeIndex = static_cast<int>(index);
}
void ModelViewer::ModeAnimationEditor::UpdateDelete(size_t index) {
	auto it = modelViewer->animations.animations.begin();
	it += index;
	modelViewer->animations.animations.erase(it);
}
void ModelViewer::ModeAnimationEditor::UpdateNameEditor(size_t index) {
	reNaming = true;
	animeIndex = static_cast<int>(index);

	nameEditor.newName = modelViewer->animations.animations.at(index).name;
}


void ModelViewer::MaterialEditor::Update() {
	UpdateRenameFile();
	UpdateMaterialList();
}
void  ModelViewer::MaterialEditor::UpdateRenameFile() {
	if (reNaming) {
		// Gui is positioning center.
		ImVec2 wh = { 800.f, 100.0f };
		ImGui::SetNextWindowPos(ImVec2(1280 - wh.x, 720 - wh.y), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(wh.x, wh.y), ImGuiSetCond_Always);

		ImGui::Begin("Rename");
		if (ImGui::Button("OK")) {
			// model's material name[i] = input name;
			modelViewer->model->materials.at(pathEditor.renameMaterialKey).textureFileName[pathEditor.renameFilePathIndex] = pathEditor.newName.c_str();
			reNaming = false;
		} ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			reNaming = false;
		}

		ImGui::InputText("new name", pathEditor.newName.data(), pathEditor.nameSizeMax);
		ImGui::End();

		return;
	}
}
void ModelViewer::MaterialEditor::UpdateMaterialList() {
	ImGui::Begin("MaterialList");

	auto b = modelViewer->model->materials.begin();
	auto e = modelViewer->model->materials.end();
	for (auto it = b; it != e; ++it) {
		// get material name
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_::ImGuiCond_Once);
		if (ImGui::TreeNode(it->second.name.c_str())) {
			ImGui::SliderFloat4("AmbiC", &it->second.Ka.x, 0.0f, 1.0f);
			ImGui::SliderFloat4("SpecC", &it->second.Ks.x, 0.0f, 1.0f);
			ImGui::SliderFloat4("DiffC", &it->second.Kd.x, 0.0f, 1.0f);


			for (size_t i = 0; i < 4; ++i) {
				// if size is not 0.
				if (it->second.textureFileName[i].size()) {
					if (ImGui::Button(it->second.textureFileName[i].c_str())) {
						reNaming = true;
						pathEditor.newName = it->second.textureFileName[i].c_str();
						pathEditor.renameMaterialKey = it->first;
						pathEditor.renameFilePathIndex = i;
					}
				}
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void ModelViewer::MaterialEditor::Render() {
	modelViewer->model->Render(modelViewer->renderSystem, modelViewer->transform.transform , Vec4(1.0f), nullptr);
}