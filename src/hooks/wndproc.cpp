#include "wndproc.h"
#include "core/globals.h"
#include "utils/Logger.h"

namespace WndProcHook {
    WNDPROC oWndProc = nullptr;
    HWND targetWindow = nullptr;

    LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_KEYDOWN:
            if (wParam == Globals::g_MenuKey) {
                Globals::g_MenuOpen = !Globals::g_MenuOpen;
                return 0;
            }
            break;
        }

        return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
    }

    void Initialize() {
        LOG_INFO("Initializing WndProc hook");
        targetWindow = GetForegroundWindow();

        if (targetWindow) {
            oWndProc = (WNDPROC)SetWindowLongPtr(targetWindow, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
            if (oWndProc) {
                LOG_SUCCESS("WndProc hooked successfully");
            }
            else {
                LOG_ERROR("Failed to hook WndProc");
            }
        }
        else {
            LOG_ERROR("Could not find target window");
        }
    }

    void Shutdown() {
        if (targetWindow && oWndProc) {
            SetWindowLongPtr(targetWindow, GWLP_WNDPROC, (LONG_PTR)oWndProc);
            LOG_INFO("WndProc hook removed");
        }
    }
}