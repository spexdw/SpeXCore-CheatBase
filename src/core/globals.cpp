#include "globals.h"

namespace Globals {
    const char* APP_NAME = "SpeX-Core";
    const char* APP_VERSION = "1.0.0";
    const char* BUILD_DATE = __DATE__ " " __TIME__;

    HWND g_hWnd = nullptr;
    bool g_Running = true;
    bool g_ShowMenu = true;

    ID3D11Device* g_pd3dDevice = nullptr;
    ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
    IDXGISwapChain* g_pSwapChain = nullptr;
    ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

    int g_MenuKey = VK_INSERT;
    bool g_MenuOpen = false;

    const char* TAHOMA_FONT_URL = "https://github.com/bpampuch/pdfmake/raw/refs/heads/master/examples/fonts/Roboto-Regular.ttf";
    const char* FONTAWESOME_URL = "https://github.com/FortAwesome/Font-Awesome/raw/refs/heads/6.x/webfonts/fa-solid-900.ttf";

    namespace Features {
        bool g_Aimbot = false;
        bool g_ESP = false;
        bool g_Wallhack = false;
        bool g_NoRecoil = false;
        bool g_SpeedHack = false;
        float g_SpeedMultiplier = 2.0f;
    }
}