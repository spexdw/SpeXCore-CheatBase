#pragma once
#include <windows.h>
#include <d3d11.h>
#include <string>

namespace Globals {
    extern const char* APP_NAME;
    extern const char* APP_VERSION;
    extern const char* BUILD_DATE;

    extern HWND g_hWnd;
    extern bool g_Running;
    extern bool g_ShowMenu;

    extern ID3D11Device* g_pd3dDevice;
    extern ID3D11DeviceContext* g_pd3dDeviceContext;
    extern IDXGISwapChain* g_pSwapChain;
    extern ID3D11RenderTargetView* g_mainRenderTargetView;

    extern int g_MenuKey;
    extern bool g_MenuOpen;

    extern const char* TAHOMA_FONT_URL;
    extern const char* FONTAWESOME_URL;

    namespace Features {
        extern bool g_Aimbot;
        extern bool g_ESP;
        extern bool g_Wallhack;
        extern bool g_NoRecoil;
        extern bool g_SpeedHack;
        extern float g_SpeedMultiplier;
    }
}