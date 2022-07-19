#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>

// #include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

#include "misc.h"
#include "high_resolution_timer.h"

#include "RenderSystem.h"

#include "UseImGui.h"

#include "sprite.h"
#include "Texture2d.h"

#include "GeometricPrimitives.h"
#include "staticMesh.h"
#include "SkinnedMesh.h"

#include "Camera.h"


class SystemWork {
public:		// for RunSystems
	void SystemInit() {
		msg = {};

		if (!initialize())
		{
			return;
		}
		IGfilter(ImGuiInit(hwnd, renderSystem.device.Get(), renderSystem.immediateContext.Get()); )
	}
	UINT GetWindowMessage() { return msg.message; }
	void GetWindowMessage(UINT message) { msg.message = message; }
	MSG  GetMSG() { return msg; }
	UINT MessageLoop() {
		while (PeekMessage(
			&msg,				// メッセージ情報を受け取るMSG構造体へのポインター
			NULL,				// メッセージを取得するウィンドウへのハンドル
			0,					// 検査されるメッセージの範囲内の最初のメッセージの値
			0,					// 最後のメッセージの値
			PM_REMOVE			// メッセージの処理方法を指定　今回は処理後にキューから削除
		))
		{
			if (msg.message == WM_QUIT) { return WM_QUIT; }
			TranslateMessage(&msg);	// 仮想キーメッセージを文字メッセージに変換
			DispatchMessage(&msg);	// ウィンドウプロシージャにメッセージを創出
		}
		tictoc.tick();
		calculateFrameStats();

		SetupRender();

		return GetWindowMessage();
	}
	int SystemUnInit() {

		IGfilter(ImGuiUnInit();)

#if 1
		BOOL fullscreen = 0;
		renderSystem.swapChain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			renderSystem.swapChain->SetFullscreenState(FALSE, 0);
		}
#endif

		return uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}
	void Present() {
		UINT syncInterval = 0;
		renderSystem.swapChain->Present(
			syncInterval,		// フレーム表示を垂直ブランクと同期する方法を指定する整数
			0					// スワップチェーン表示オプションを含む整数値　今回は各バッファ (現在のバッファから始まる) から出力にフレームを表示
		);
	}
	float ElapsedTime() { return tictoc.time_interval(); }

	void Render();

public:	// COM object and renderDesks
	RenderSystem renderSystem;

private: //geometric


	// std::unique_ptr<GeometricPrimitive> geometricPrimitives[8];
	// std::unique_ptr<StaticMesh>			staticMeshes[8];
	
	AnimationManager					animationManager;
	SkinMeshManager						skinMeshManager;

	Transform							nicoTransform;
	Transform							monsterTransform;

private:	// window data
	MSG msg;

	const int screenWidth;
	const int screenHeight;

	HWND hwnd; //WiNDHNDle
public:
	HWND GetWindowHandle() { return hwnd; }

private:	// texture data
	Texture2dManager				textures;
	std::unique_ptr<sprite>	text;
public:
	Texture2dManager*	texturesP() { return &textures; }
	void Textout(std::string string, float x, float y, float w, float h, float r, float g, float b, float a) {
		float sw = static_cast<float>(text->Width() / 16);
		float sh = static_cast<float>(text->Height() / 16);
		float carriage = 0.0f;
		for (const char c : string) {
			text->render(renderSystem.immediateContext.Get(), x + carriage, y, w, h, r, g, b, a, 0, sw * (c & 0x0f), sh * (c >> 4), sw, sh);
			carriage += w;
		}
	}

private: // about render function
	void SetupRender();

public:		
	SystemWork(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show, int _screenWidth, int _screenHeight);
	~SystemWork();

	SystemWork(const SystemWork&) = delete;
	SystemWork& operator=(const SystemWork&) = delete;
	SystemWork(SystemWork&&) noexcept = delete;
	SystemWork& operator=(SystemWork&&) noexcept = delete;

public:		// return message
	LRESULT CALLBACK handleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
IGfilter(
		if (ImGui_ImplWin32_WndProcHandler(hwnd,msg,wparam,lparam)) { return true; }
)
	switch (msg) {
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		// if (GetAsyncKeyState(VK_ESCAPE)) {
		// 	PostMessage(hwnd, WM_CLOSE, 0, 0);
		// }
		break;
	case WM_ENTERSIZEMOVE:
		tictoc.stop();
		break;
	case WM_EXITSIZEMOVE:
		tictoc.start();
		break;

	default: 
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

		return 0;
	}

private:
	bool initialize();
	bool uninitialize();

private:	// util
	high_resolution_timer tictoc; // 高解像度のタイマー
	uint32_t frames = 0;
	float elapsed_time = 0.0f;
	void calculateFrameStats();
};

