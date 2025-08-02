#pragma once
#include <d3d11.h>
#include <dxgi.h>

namespace DirectX {
    void Initialize();
    void Shutdown();
    void RenderMenu();

    bool CreateRenderTarget();
    void CleanupRenderTarget();
    void CleanupDevice();
    bool InitializeImGui();
    void ShutdownImGui();
    bool HookVTable(void** vtable, int index, void* hook, void** original);

    extern ID3D11Device* g_pd3dDevice;
    extern ID3D11DeviceContext* g_pd3dDeviceContext;
    extern ID3D11RenderTargetView* g_mainRenderTargetView;
    extern IDXGISwapChain* g_pSwapChain;
    extern bool init;
    extern bool device_lost;
}