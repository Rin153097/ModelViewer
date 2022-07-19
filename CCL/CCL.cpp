#include "CCL.h"

namespace CCL {
    static std::vector<SystemWork*> windows;

    SystemWork* GetWindow(int windowIndex) { return windows.at(windowIndex); }

    int BuildWindow(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show, int screenWidth, int screenHeight) {
        SystemWork* w = windows.emplace_back(new SystemWork(instance, prev_instance, cmd_line, cmd_show, screenWidth, screenHeight));
        w->SystemInit();
        return static_cast<int>(windows.size() - 1);
    }
    UINT GameLoop(int windowIndex) {
        UINT R = windows.at(windowIndex)->MessageLoop();

        return R;
    }
    int EndSystem(int windowIndex) {
        int r = windows.at(windowIndex)->SystemUnInit();
        delete windows.at(windowIndex);
        return r;
    }

    float GetElapsedTime(int windowIndex) {
        return windows.at(windowIndex)->ElapsedTime();
    }
    void SetupRender(int windowIndex) {
        windows.at(windowIndex)->renderSystem.SetupRender();
    }
    void Present(int windowIndex) {
        windows.at(windowIndex)->Present();
    }

    void SetViewProjection(Matrix viewProjection, int bufferIndex, int windowIndex) {
        windows.at(windowIndex)->renderSystem.SetConstantViewProjection(viewProjection, bufferIndex);
    }
    void SetCameraPosition(Vec3 cameraPosition, int bufferIndex, int windowIndex) {
        windows.at(windowIndex)->renderSystem.SetConstantCameraPosition(cameraPosition, bufferIndex);
    }
    void SetAmbientLight(Vec4 ambientLight, int bufferIndex, int windowIndex) {
        windows.at(windowIndex)->renderSystem.SetConstantAmbientLight(ambientLight, bufferIndex);

    }
    void SetLightDirect(Vec3 lightDirect, int bufferIndex, int windowIndex) {
        windows.at(windowIndex)->renderSystem.SetConstantLightDirection(lightDirect, bufferIndex);
    }

    MSG GetMSG(int windowIndex) {
        return windows.at(windowIndex)->GetMSG();
    }
    void SetWindowMessage(UINT Message, int windowIndex) {
        windows.at(windowIndex)->GetWindowMessage(Message);
    }

    RenderSystem* GetRenderSystem(int windowIndex) {
        return &windows.at(windowIndex)->renderSystem;
    }
}