#pragma once

#include "framework.h"

namespace CCL {
    SystemWork* GetWindow(int windowIndex = 0);

    int BuildWindow(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show, int screenWidth, int screenHeight);
    UINT GameLoop(int windowIndex = 0);
    int EndSystem(int windowIndex = 0);


    float GetElapsedTime(int windowIndex = 0);
    void SetupRender(int windowIndex = 0);
    void Present(int windowIndex = 0);

    void SetViewProjection(Matrix viewProjection, int index = 0, int windowIndex = 0);
    void SetCameraPosition(Vec3 cameraPosition, int index = 0, int windowIndex = 0);
    void SetAmbientLight(Vec4 ambientLight, int index = 0, int windowIndex = 0);
    void SetLightDirect(Vec3 lightDirect, int index = 0, int windowIndex = 0);

    MSG GetMSG(int windowIndex = 0);
    void SetWindowMessage(UINT Message, int windowIndex = 0);

    RenderSystem* GetRenderSystem(int windowIndex = 0);
}