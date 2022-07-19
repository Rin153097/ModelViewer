#include "framework.h"

#ifdef CCL_IMGUI

static int ImGuiCallCreateContextCount = 0;
void ImGuiInit(HWND handle, ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
	if (!ImGuiCallCreateContextCount) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
		ImGui_ImplWin32_Init(handle);
		ImGui_ImplDX11_Init(device, deviceContext);
		ImGui::StyleColorsDark();
	}
	++ImGuiCallCreateContextCount;
}
void ImGuiUnInit() {
	--ImGuiCallCreateContextCount;
	if (!ImGuiCallCreateContextCount) {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();

		ImGui::DestroyContext();
	}
}
bool ImGuiHandler(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) {
	return ImGui_ImplWin32_WndProcHandler(handle, msg, wparam, lparam);
}
void ImGuiBegin() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
void ImGuiEnd() {
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

#endif