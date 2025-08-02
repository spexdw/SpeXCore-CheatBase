#include "directx.h"
#include "core/globals.h"
#include "utils/Logger.h"
#include "utils/FontManager.h"
#include "gui/menu.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <dxgi.h>

extern unsigned char Inter_SemmiBold[];
extern unsigned char Inter_Bold[];
extern unsigned char Icon_Pack[];
extern unsigned char Arrow[];

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace DirectX {
    typedef HRESULT(__stdcall* Present)(IDXGISwapChain*, UINT, UINT);
    typedef HRESULT(__stdcall* ResizeBuffers)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

    Present oPresent = nullptr;
    ResizeBuffers oResizeBuffers = nullptr;
    WNDPROC oWndProc = nullptr;
    HWND window = nullptr;
    ID3D11Device* g_pd3dDevice = nullptr;
    ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
    ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
    IDXGISwapChain* g_pSwapChain = nullptr;
    bool init = false;
    bool device_lost = false;

    LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_KEYDOWN && wParam == Globals::g_MenuKey) {
            Globals::g_MenuOpen = !Globals::g_MenuOpen;
            LOG_INFO("Menu toggled: " + std::string(Globals::g_MenuOpen ? "ON" : "OFF"));
            return 0;
        }

        if (init && ImGui::GetCurrentContext()) {
            if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
                return true;
        }

        return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
    }

    bool CreateRenderTarget() {
        if (!g_pSwapChain || !g_pd3dDevice) return false;

        CleanupRenderTarget();

        ID3D11Texture2D* pBackBuffer = nullptr;
        HRESULT hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (FAILED(hr)) {
            LOG_ERROR("Failed to get back buffer");
            return false;
        }

        hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
        pBackBuffer->Release();

        if (FAILED(hr)) {
            LOG_ERROR("Failed to create render target view");
            return false;
        }

        LOG_DEBUG("Render target created successfully");
        return true;
    }

    void CleanupRenderTarget() {
        if (g_mainRenderTargetView) {
            g_mainRenderTargetView->Release();
            g_mainRenderTargetView = nullptr;
        }
    }

    void CleanupDevice() {
        CleanupRenderTarget();

        if (g_pd3dDeviceContext) {
            g_pd3dDeviceContext->Release();
            g_pd3dDeviceContext = nullptr;
        }

        if (g_pd3dDevice) {
            g_pd3dDevice->Release();
            g_pd3dDevice = nullptr;
        }

        g_pSwapChain = nullptr;
        device_lost = true;
    }

    bool DirectX::InitializeImGui() {
        if (!g_pd3dDevice || !window) return false;

        if (!ImGui::GetCurrentContext()) {
            ImGui::CreateContext();
            LOG_DEBUG("ImGui context created");
        }

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;

        FontManager::LoadFontsToImGui();

        FontManager::LoadFont("Inter", 17.0f);
        FontManager::LoadFont("Inter", 18.0f);
        FontManager::LoadFont("Inter", 23.0f);
        FontManager::LoadFont("Inter", 15.0f);
        FontManager::LoadFont("Inter", 34.0f);

        Inter_S = FontManager::GetFont("Inter", 17.0f);
        Inter_S_1 = FontManager::GetFont("Inter", 18.0f);
        Inter_S_2 = FontManager::GetFont("Inter", 23.0f);
        Inter_S_3 = FontManager::GetFont("Inter", 15.0f);
        Inter_B = FontManager::GetFont("Inter", 34.0f);

        if (Inter_S) {
            io.FontDefault = Inter_S;
        }

        if (!ImGui_ImplWin32_Init(window)) {
            LOG_ERROR("Failed to initialize ImGui Win32");
            return false;
        }

        if (!ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext)) {
            LOG_ERROR("Failed to initialize ImGui DX11");
            ImGui_ImplWin32_Shutdown();
            return false;
        }

        LOG_SUCCESS("ImGui initialized successfully");
        return true;
    }

    void ShutdownImGui() {
        if (ImGui::GetCurrentContext()) {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }
    }

    HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
        static bool first_call = true;
        if (first_call) {
            LOG_INFO("Present hook called for first time");
            first_call = false;
        }

        if (!init || device_lost) {
            CleanupDevice();

            if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3dDevice))) {
                g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);

                DXGI_SWAP_CHAIN_DESC sd;
                pSwapChain->GetDesc(&sd);
                HWND newWindow = sd.OutputWindow;

                if (newWindow != window) {
                    if (window && oWndProc) {
                        SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
                    }

                    window = newWindow;
                    oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
                    LOG_INFO("Window handle updated: " + std::to_string((uintptr_t)window));
                }

                g_pSwapChain = pSwapChain;

                if (CreateRenderTarget()) {
                    if (!init) {
                        if (InitializeImGui()) {
                            init = true;
                            device_lost = false;
                            LOG_SUCCESS("DirectX hooks initialized successfully");
                        }
                    }
                    else {
                        device_lost = false;
                        LOG_SUCCESS("Device recovered");
                    }
                }
            }
        }

        if (init && !device_lost && g_mainRenderTargetView && ImGui::GetCurrentContext()) {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            RenderMenu();

            ImGui::EndFrame();
            ImGui::Render();

            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }

        HRESULT result = oPresent(pSwapChain, SyncInterval, Flags);

        if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET) {
            LOG_WARNING("Device lost detected");
            device_lost = true;
        }

        return result;
    }

    HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        if (init && pSwapChain == g_pSwapChain) {
            CleanupRenderTarget();
            LOG_INFO("Buffers resized, cleaning render target");
        }

        HRESULT result = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

        if (init && pSwapChain == g_pSwapChain && SUCCEEDED(result)) {
            CreateRenderTarget();
        }

        return result;
    }

    void DirectX::RenderMenu() {
        try {
            if (Globals::g_MenuOpen && Inter_S) {
                Menu::Render();
            }
            if (Inter_S) {
                Menu::RenderWatermark();
            }
        }
        catch (...) {
            LOG_ERROR("Exception in RenderMenu");
        }
    }

    bool HookVTable(void** vtable, int index, void* hook, void** original) {
        if (!vtable || !hook) return false;

        DWORD oldProtect;
        if (!VirtualProtect(&vtable[index], sizeof(void*), PAGE_READWRITE, &oldProtect)) {
            return false;
        }

        if (original) *original = vtable[index];
        vtable[index] = hook;

        VirtualProtect(&vtable[index], sizeof(void*), oldProtect, &oldProtect);
        return true;
    }

    void Initialize() {
        LOG_INFO("Initializing DirectX hooks");

        HWND tempWindow = CreateWindowA("BUTTON", "Temp Window", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
        if (!tempWindow) {
            LOG_ERROR("Failed to create temporary window");
            return;
        }

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        swapChainDesc.BufferCount = 1;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = tempWindow;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL featureLevel;
        IDXGISwapChain* tempSwapChain = nullptr;
        ID3D11Device* tempDevice = nullptr;
        ID3D11DeviceContext* tempContext = nullptr;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
            &tempSwapChain, &tempDevice, &featureLevel, &tempContext
        );

        if (FAILED(hr)) {
            LOG_ERROR("Failed to create temporary D3D11 device");
            DestroyWindow(tempWindow);
            return;
        }

        void** swapChainVTable = *reinterpret_cast<void***>(tempSwapChain);

        if (HookVTable(swapChainVTable, 8, &hkPresent, (void**)&oPresent)) {
            LOG_SUCCESS("Present hooked successfully");
        }
        else {
            LOG_ERROR("Failed to hook Present");
        }

        if (HookVTable(swapChainVTable, 13, &hkResizeBuffers, (void**)&oResizeBuffers)) {
            LOG_SUCCESS("ResizeBuffers hooked successfully");
        }
        else {
            LOG_ERROR("Failed to hook ResizeBuffers");
        }

        tempSwapChain->Release();
        tempDevice->Release();
        tempContext->Release();
        DestroyWindow(tempWindow);

        LOG_SUCCESS("DirectX hooks setup complete");
    }

    void Shutdown() {
        if (init) {
            ShutdownImGui();
            init = false;
        }

        if (window && oWndProc) {
            SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
        }

        CleanupDevice();
        LOG_INFO("DirectX hooks shutdown");
    }
}