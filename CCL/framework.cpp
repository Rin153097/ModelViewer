#include "framework.h"

CONST BOOL FULLSCREEN = FALSE;
CONST LPCWSTR APPLICATION_NAME = L"X3DGP";

inline LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    SystemWork* p = reinterpret_cast<SystemWork*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    return p ? p->handleMessage(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
}

SystemWork::SystemWork(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show, int _screenWidth, int _screenHeight)
    :
    screenWidth(_screenWidth),
    screenHeight(_screenHeight)
{
    srand(static_cast<unsigned int>(time(nullptr)));

    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = window_procedure;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instance;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = APPLICATION_NAME;
    wcex.hIconSm = 0;
    RegisterClassExW(&wcex);

    RECT rc = { 0, 0, screenWidth, screenHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    hwnd = CreateWindowExW(0, APPLICATION_NAME, L"", WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);
    ShowWindow(hwnd, cmd_show);

    renderSystem = RenderSystem(screenWidth, screenHeight, hwnd, FULLSCREEN);

#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(####);
#endif
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

bool SystemWork::initialize()
{
    textures.SetRenderSystem(&renderSystem);

    // textures.LoadSprite(L".\\resources\\screenshot.jpg", 1, Vec2(1600.0f, 900.0f), 1);
    // staticMeshes[0] = std::make_unique<StaticMesh>(device.Get(), L".\\resources\\Bison\\Bison.obj",true);
    // staticMeshes[0] = std::make_unique<StaticMesh>(device.Get(), L".\\resources\\Cup\\Cup.obj",true);
    animationManager.animations.emplace_back(Animation());

    skinMeshManager.SetRenderSystem(&renderSystem);
    // skinMeshManager.LoadMesh(animationManager.animations, ".\\resources\\nico.fbx", 0.0f, false);
    // skinMeshManager.LoadMesh(animationManager.animations, ".\\resources\\Monster\\Monster.fbx", 0.0f, true, RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, Vec4(1.0f));
    // skinMeshManager.LoadMesh(animationManager.animations, ".\\resources\\PlayerFlap\\flap.fbx", 0.0f, true, RASTERIZER_STATE::FILL_AND_CULL, BLEND_TYPE::ALPHA, DEPTH_STATE::ENABLE_AND_WRITE, Vec4(1.0f));
    // skinnedMeshes[0] = std::make_unique<SkinMesh>(animationManager.animations, renderSystem.device.Get(), ".\\resources\\nico.fbx", 0.0f, false);
    // skinnedMeshes[0] = std::make_unique<SkinMesh>(device.Get(), ".\\resources\\plantune.fbx", 0.0f, false);

    nicoTransform = Transform(false, Vec3(1.0f), Vec4::QuaternionIdentity(), Vec3(-3.0f,0.0f,0.0f));
    DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 3.14f);
    Vec4 q;
    DirectX::XMStoreFloat4(&q, Q);
    monsterTransform = Transform(false, Vec3(0.01f), q, Vec3(0.0f, 0.0f, 0.0f));

    return true;
}

void SystemWork::calculateFrameStats() {
    if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
    {
        float fps = static_cast<float>(frames);
        std::wostringstream outs;
        outs.precision(6);
        outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
        SetWindowTextW(hwnd, outs.str().c_str());

        frames = 0;
        elapsed_time += 1.0f;
    }
}

void SystemWork::SetupRender() {
    SceneConstants::Data* sc = renderSystem.constantBuffer.GetBufferData(0);
    renderSystem.SetupRender(Vec4(0.0f, 0.0f, 1.0f, 1.0f), 0);
#if 0
    immediateContext->PSSetSamplers(
        0,											// デバイスの0から始まる配列にインデックスをつけサンプラーの設定を開始する
        1,											// 配列内のサンプラー数
        renderStates.samplerStates.GetAddressOf(SamplerStates::STATE::POINT)							// サンプラー状態のインターフェイスの配列へのポインタ
    );
    immediateContext->PSSetSamplers(1, 1,
        renderStates.samplerStates.GetAddressOf(SamplerStates::STATE::LINEAR));
    immediateContext->PSSetSamplers(2, 1,
        renderStates.samplerStates.GetAddressOf(SamplerStates::STATE::ANISOTROPIC));
#endif

}

void SystemWork::Render() {
    using namespace DirectX;

    // textures.Begin(0);
    // textures.Render(0, Texture2d::DrawData(Vec2(), Vec2(1280.0f, 720.0f), 0, 0.0f, Vec4(1.0f)));
    // textures.End(0);


    // static int clipIndex = 1;
    // int frameIndex = 0;
    // static float animationTick = 0;
    // 
    // static float blendFactor = 0.5f;
    // 
    // IGfilter(
    //     ImGui::Begin("Animation");
    //     ImGui::InputInt("AnimationKind", &clipIndex);
    // 
    //     ImGui::SliderFloat("blendFactor", &blendFactor, 0.0f, 1.0f);
    // )
    // if (clipIndex < 0) {
    //         clipIndex = 0;
    //     }
    // if (clipIndex >= animationManager.animations.size()) { clipIndex = static_cast<int>(animationManager.animations.size() - 1); }
    // 
    // Animation& animation = animationManager.animations.at(clipIndex);
    // 
    // frameIndex = static_cast<int>(animationTick * animation.samplingRate);
    // if (frameIndex > animation.sequence.size() - 1) {
    //     frameIndex = 0;
    //     animationTick = 0;
    // }
    // else {
    //     animationTick += ElapsedTime();
    // }
    // Animation::KeyFrame& keyframe = animation.sequence.at(frameIndex);


    // MonsterAnimation
    // const int mClipIndex = 9;
    // Animation& mAnimation = animationManager.animations.at(mClipIndex);

    // static float mAnimationTick;
    // 
    // int mFrameIndex = static_cast<int>(mAnimationTick * mAnimation.samplingRate);
    // if (mFrameIndex > mAnimation.sequence.size() - 1) {
    //     mFrameIndex = 0;
    //     mAnimationTick = 0;
    // }
    // else {
    //     mAnimationTick += ElapsedTime();
    // }
   // static float flap = 0.0f;
   // IGfilter(
   //     ImGui::Begin("Player");
   //     ImGui::SliderFloat("FLAP", &flap, -1.0f, +1.0f);
   //     ImGui::End();
   // )
   // Animation::KeyFrame& sKeyframe = mAnimation.sequence.at(0);
   //
   // Animation::KeyFrame& eKeyframe = (flap > 0.0f) ? mAnimation.sequence.at(1) : mAnimation.sequence.at(2);
   //
   // Animation::KeyFrame oKeyframe;
   // SkinMesh::BlendAnimations(oKeyframe, &sKeyframe, &eKeyframe, fabsf(flap));
   // SkinMesh::UpdateAnimation(oKeyframe, skinMeshManager.Get(1));
    // MonsterAnimation

    // IGfilter(
    //     ImGui::Text(animation.name.c_str());
    //     ImGui::InputInt("frameIndex", &frameIndex);
    //     ImGui::End();
    // )
#if 1
    // skinMeshManager.UpdateAnimation(0, keyframe);
    // skinnedMeshes[0]->UpdateAnimation(keyframe);
#else
    const Animation::KeyFrame* keyframes[2] = {
        &animationManager.animations.at(0).sequence.at(0),
        &animationManager.animations.at(0).sequence.at(animationManager.animations.at(clipIndex).sequence.size()),
    };
    SkinMesh::BlendAnimations(tmpKeyframe, keyframes[0], keyframes[1], blendFactor);
    // skinnedMeshes[0]->UpdateAnimation(keyframe);
#endif
    // skinMeshManager.Render(0, nullptr, nicoTransform.transform, Vec4(1.0f));
    // skinMeshManager.Render(1, &oKeyframe, monsterTransform.transform, Vec4(1.0f));
}

bool SystemWork::uninitialize()
{
    return true;
}

SystemWork::~SystemWork()
{

}