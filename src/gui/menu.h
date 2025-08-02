#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_settings.h"
#include "custom_widgets.h"
#include <icons.h>
#include <d3d11.h>

void CustomStyleColor();

extern ID3D11ShaderResourceView* bg;
extern ImFont* Inter_S;
extern ImFont* Inter_S_1;
extern ImFont* Inter_S_2;
extern ImFont* Inter_S_3;
extern ImFont* Inter_B;
extern ImFont* Icon;
extern ImFont* Icon_Arrow;

extern int sub_page;
extern int page;
extern float tab_alpha;
extern float tab_add;
extern int active_tab;
extern int togle;
extern float anim_text;

namespace Menu {
    void Initialize();
    void Shutdown();
    void Render();
    void RenderWatermark();
}