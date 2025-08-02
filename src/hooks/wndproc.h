#pragma once
#include <windows.h>

namespace WndProcHook {
    void Initialize();
    void Shutdown();

    extern WNDPROC oWndProc;
    extern HWND targetWindow;
}