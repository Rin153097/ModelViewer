#pragma once

// #undef USE_IMGUI

#define CCL_IMGUI TRUE


#ifdef CCL_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

#ifdef CCL_IMGUI
#define IGfilter(any) any
#else
#define IGfilter(any)
#endif

#ifdef CCL_IMGUI
#define ImGuiCreateContext(handle,device,deviceContext)     ImGui::CreateContext()
#define ImGuiDestroyContext(handle,device,deviceContext)    ImGui::DestroyContext()
#define ImGuiSetup()                                        ImGuiBegin()
#define ImGuiRender()                                       ImGuiEnd()
#else
#define ImGuiCreateContext()
#define ImGuiDestroyContext()
#define ImGuiSetup() 
#define ImGuiRender()
#endif


#include <vector>
#include "Template/Singleton.h"


#ifdef CCL_IMGUI
void ImGuiInit(HWND handle, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
void ImGuiUnInit();

void ImGuiBegin();
void ImGuiEnd();
bool ImGuiHandler(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);
#endif