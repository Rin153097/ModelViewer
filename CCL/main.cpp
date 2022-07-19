#include <time.h>

#include "framework.h"
#include "CCL.h"

#include "DirectionalLight.h"

#include "InputPrimitive.h"

#include "Template/inlines.h"
#include "PrimitiveModel.h"

#include "Primitive2D.h"

#include "TextManager.h"

#include "../ModelViewer/ModelViewer.h"

#define DEBUG_OFFSCREEN 0

int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_  HINSTANCE prev_instance, _In_ LPSTR cmd_line, _In_ int cmd_show)
{
	using namespace CCL;
	// SystemWork systemwork(instance, prev_instance, cmd_line, cmd_show, 1280, 720);
	
	const int ScreenWidth = 1280;
	const int ScreenHeight = 720;

	BuildWindow(instance, prev_instance, cmd_line, cmd_show, ScreenWidth, ScreenHeight);

	// SystemWork second(instance, prev_instance, cmd_line, cmd_show, 500, 100);
	// second.SystemInit();

	// systemwork.SystemInit();

	CameraUseAngle camera;
	CameraController cameraController(&camera);

	DirectionalLight light;

	// Texture2dManager textures(GetRenderSystem());
	// textures.LoadSprite(L"./resources/screenshot.jpg",1,Vec2(1600.0f,900.0f),1);

	ModelViewer modelViewer(GetRenderSystem());
	// std::unique_ptr<SkinMesh> nico = std::make_unique<SkinMesh>(nullptr, GetRenderSystem()->device.Get(), "./resources/nico.fbx"
	// 	,0.0f, true, COORDINATE_TYPE::RIGHT_YUP, RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE);

	// const int BackGround = textures.LoadSprite(L".\\resources\\screenshot.jpg", 1, Vec2(1600.0f, 900.0f), 1);
	// skinMeshes.LoadMesh(animations.animations, ".\\resources\\FlapRotateInverse.fbx", 0.0f, true, COORDINATE_TYPE::RIGHT_YUP, Vec4(0.5f), RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, true);
	// skinMeshes.LoadMesh(animations.animations, "./resources/PlayerFlap/flap.fbx", 0.0f, true, COORDINATE_TYPE::RIGHT_YUP, Vec4(0.5f), RASTERIZER_STATE::FILL_NOT_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, false);

	Transform meshTransform(false,Vec3(0.01f),Vec4::QuaternionIdentity(),Vec3());
	mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -1, 0);


	// Init Camera
	{
		camera.AllReset();

		camera.aspect = static_cast<float>(ScreenWidth) / static_cast<float>(ScreenHeight);
		camera.fovAngle = DirectX::XMConvertToRadians(30);
		camera.nearZ = 0.1f;
		camera.farZ = 100.0f;

		camera.eye = Vec3(0.0f, 0.0f, 0.0f);
		camera.focus = Vec3(0.0f, 0.0f, 0.0f);
		camera.up = Vec3(0.0f, 1.0f, 0.0f);

		camera.angle = {};

		camera.range = 10.0f;

		camera.UpdateAllCameraMatrix();
	}

	Cursor::Init();
	KeyboardManager::Instance().Init();

	Vec2 drawPos = {};
	Vec2 drawWH = { 640.0f, 360.0f };
	bool activeLog = false;
	bool lockWindowPos = false;

	while (GameLoop() != WM_QUIT) {
		ImGuiSetup();
		Cursor::Update();
		KeyboardManager::Instance().Update();

		// // ShowCursor
		// if (KeyboardManager::Instance().Trg(CCL_K_UP))		{ Cursor::ShowMouseCursor(TRUE); }
		// if (KeyboardManager::Instance().Trg(CCL_K_DOWN))	{ Cursor::ShowMouseCursor(FALSE); }

		// update
		{
			cameraController.Update();
			camera.aspect = drawWH.x / drawWH.y;
			camera.Update();
			light.Update();

			modelViewer.Update(GetElapsedTime());
		}

		// render
		{
			// render in offScreen0

#if  DEBUG_OFFSCREEN
			CCL::GetRenderSystem()->frameBuffers[0]->Clear(CCL::GetRenderSystem()->immediateContext.Get(), 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			CCL::GetRenderSystem()->frameBuffers[0]->Activate(CCL::GetRenderSystem()->immediateContext.Get());
#endif
			// modelViewer.Render(ScreenWidth, ScreenHeight, camera.GetProjectionMatrix(), camera.GetViewMatrix());
			// textures.Begin(0);
			// textures.Render(0, Texture2d::DrawData(Vec2(), Vec2(1280.0f, 720.0f), 0, 0.0f));
			// textures.End(0);
			// 
			// nico->Render(CCL::GetRenderSystem(), Transform(false, Vec3(0.01)).transform);
			modelViewer.Render(ScreenWidth, ScreenHeight, camera.GetProjectionMatrix(),camera.GetViewMatrix());

#if  DEBUG_OFFSCREEN
			CCL::GetRenderSystem()->frameBuffers[0]->DeActivate(CCL::GetRenderSystem()->immediateContext.Get());
#endif
			// end render
#if 0
			// render in offScreen1
			GetRenderSystem()->SwitchRasterizer(RASTERIZER_STATE::FILL_AND_CULL);
			GetRenderSystem()->SwitchDepthStencil(DEPTH_STATE::ENABLE_NOT_WRITE);
			GetRenderSystem()->frameBuffers[1]->Clear(GetRenderSystem()->immediateContext.Get());
			GetRenderSystem()->frameBuffers[1]->Activate(GetRenderSystem()->immediateContext.Get());

			GetRenderSystem()->bitBlockTransfer->blit(GetRenderSystem()->immediateContext.Get(),
				GetRenderSystem()->frameBuffers[0]->shaderResourceViews[0].GetAddressOf(), 0, 1, GetRenderSystem()->pixelShaders[0].Get());

			GetRenderSystem()->frameBuffers[1]->DeActivate(GetRenderSystem()->immediateContext.Get());
#endif
#if  DEBUG_OFFSCREEN
			// render offScreen1 in normal

			ImGui::SetNextWindowPos(ImVec2(drawPos.x, drawPos.y), ImGuiCond_::ImGuiSetCond_Once);
			ImGui::SetNextWindowSize(ImVec2(drawWH.x, drawWH.y), ImGuiCond_::ImGuiCond_Always);

			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TitleBg , ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TitleBgActive , ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			ImGui::Begin("View", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);
			GetRenderSystem()->bitBlockWindow->blit(GetRenderSystem()->immediateContext.Get(),
				GetRenderSystem()->frameBuffers[0]->shaderResourceViews[0].GetAddressOf(), 0, 1, nullptr,
				drawPos.x, drawPos.y, drawWH.x, drawWH.y,
				1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 1.0f);
			ImGuiWindow* viewWindow = ImGui::GetCurrentWindow();
			
			// user first touch window
			if (lockWindowPos) {
				viewWindow->Pos.x = drawPos.x;
				viewWindow->Pos.y = drawPos.y;
			}
			else {
				drawPos.x = viewWindow->Pos.x;
				drawPos.y = viewWindow->Pos.y;
			}

			drawWH.x = viewWindow->Size.x;
			drawWH.y = viewWindow->Size.y;
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::End();
#endif
#if 0
			ID3D11ShaderResourceView* shaderResourceViews[2] = {
				{ GetRenderSystem()->frameBuffers[0]->shaderResourceViews[0].Get() },
				{ GetRenderSystem()->frameBuffers[1]->shaderResourceViews[0].Get() }
			};
			GetRenderSystem()->bitBlockTransfer->blit(GetRenderSystem()->immediateContext.Get(),
				shaderResourceViews, 0, 2, GetRenderSystem()->pixelShaders[1].Get());
#endif

			ImGuiRender();
			CCL::Present();
		}
	}
	// second.SystemUnInit();
	return EndSystem();
}
