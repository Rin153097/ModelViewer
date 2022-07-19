#pragma once

class InputPrimitive {
public:
    static bool State(int input) { return GetAsyncKeyState(input); }
    static int WheelDown() { return GET_WHEEL_DELTA_WPARAM(CCL::GetMSG().wParam); }
};
typedef unsigned int InputMarker;

#define CCL_K_UP        ( 0x00000001 )
#define CCL_K_DOWN      ( 0x00000002 )
#define CCL_K_RIGHT     ( 0x00000004 )
#define CCL_K_LEFT      ( 0x00000008 )
#define CCL_K_A         ( 0x00000010 )
#define CCL_K_B         ( 0x00000020 )
#define CCL_K_X         ( 0x00000040 )
#define CCL_K_Y         ( 0x00000080 )
#define CCL_K_RTRG1     ( 0x00000100 )
#define CCL_K_RTRG2     ( 0x00000200 )
#define CCL_K_LTRG1     ( 0x00000400 )
#define CCL_K_LTRG2     ( 0x00000800 )
#define CCL_K_RSTICK    ( 0x00001000 )
#define CCL_K_LSTICK    ( 0x00002000 )
#define CCL_K_START     ( 0x00004000 )
#define CCL_K_PAUSE     ( 0x00008000 )
#define CCL_K_HOME      ( 0x00010000 )

class InputData {
public:
    InputMarker data;
    unsigned int input; // example VK_LBUTTON
};

class KeyboardManager {
private:
    InputMarker trg;
    InputMarker state;
    InputMarker release;

    InputMarker past;

    std::vector<InputData> inputData;
private:
    KeyboardManager() : trg(), state(), release(), past(), inputData() {}

public:
    static KeyboardManager& Instance() { static KeyboardManager i; return i; }

public:
    bool Trg(InputMarker key)       { return trg & key; }
    bool State(InputMarker key)     { return state & key; }
    bool Release(InputMarker key)   { return release & key; }


    void Init();
    void Update() {
        size_t size = inputData.size();
        trg = 0; state = 0; release = 0;

        // GetState
        for (size_t i = 0; i < size; ++i) {
            state |= GetAsyncKeyState(inputData.at(i).input) ? inputData.at(i).data : 0;
        }
        // Get trg and release
        trg = (~past) & state;
        release = past & (~state);

        past = state;

        int t = static_cast<int>(trg);
        int s = static_cast<int>(state);
        int r = static_cast<int>(release);
    }

    void LoadInputData(InputData* data, int size) {
        for (int i = 0; i < size; ++i) {
            inputData.emplace_back(data[i]);
        }
    }
};

class Cursor {
    static POINT    screen;
    static POINT    client;
    static POINT    log;
    static POINT    delta;
    static int      showCounta;
public:
    // call before starting loop
    static void Init() {
        GetCursorPos(&screen);
        client = screen;
        // need HWND
        ScreenToClient(CCL::GetWindow()->GetWindowHandle(), &client);
        log = screen;
        delta = {};
    }
    // call in loop
    static void Update() {
        GetCursorPos(&screen);
        client = screen;
        // need HWND
        ScreenToClient(CCL::GetWindow()->GetWindowHandle(), &client);
        delta.x = screen.x - log.x;
        delta.y = screen.y - log.y;

        log = screen;
    }

    static Vec2 GetCursorScreenPos() { return Vec2(static_cast<float>(screen.x), static_cast<float>(screen.y)); }
    static Vec2 GetCursorClientPos() { return Vec2(static_cast<float>(client.x), static_cast<float>(client.y)); }
    static Vec2 GetCursorDelta() { return Vec2(static_cast<float>(delta.x), static_cast<float>(delta.y)); }

    static void ShowMouseCursor(BOOL show) {
        if (showCounta < 0) {
            if (show) showCounta = ShowCursor(TRUE);
        }
        else {
            if (!show) showCounta = ShowCursor(FALSE);
        }
    }
    static void SetCursorClientPos(POINT position) {
        SetCursorPos(position.x, position.y);
    }
    static void SetCursorScreenPos(POINT position) {
        ClientToScreen(CCL::GetWindow()->GetWindowHandle(), &position);
        SetCursorPos(position.x, position.y);
    }
};