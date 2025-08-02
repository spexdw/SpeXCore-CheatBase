#include <windows.h>
#include "core/globals.h"
#include "utils/Logger.h"
#include "utils/FontManager.h"
#include "hooks/directx.h"
#include "gui/menu.h"

DWORD WINAPI MainThread(LPVOID lpParam) {
    Logger::Initialize();
    LOG_SUCCESS("SpeX-Core CheatBase injected successfully.");

    FontManager::Initialize();

    DirectX::Initialize();
    Sleep(1000);

    Menu::Initialize();
    Sleep(1000);

    LOG_SUCCESS("SpeX-Core initialized!");

    while (Globals::g_Running) {
        Sleep(10);
    }

    LOG_WARNING("Shutting down SpeX-Core");

    Menu::Shutdown();
    DirectX::Shutdown();
    Logger::Shutdown();

    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }
    return TRUE;
}