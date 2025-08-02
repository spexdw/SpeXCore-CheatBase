#include "menu.h"
#include "core/globals.h"
#include "utils/Logger.h"
#include <chrono>

ID3D11ShaderResourceView* bg = nullptr;
ImFont* Inter_S = nullptr;
ImFont* Inter_S_1 = nullptr;
ImFont* Inter_S_2 = nullptr;
ImFont* Inter_S_3 = nullptr;
ImFont* Inter_B = nullptr;
ImFont* Icon = nullptr;
ImFont* Icon_Arrow = nullptr;

int sub_page = 0;
int page = 0;
float tab_alpha = 0.f;
float tab_add = 0.f;
int active_tab = 0;
int togle = 0;
float anim_text = 0.f;

void CustomStyleColor() {
    ImGuiStyle& s = ImGui::GetStyle();
    ImGuiContext& g = *GImGui;

    s.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
    s.Colors[ImGuiCol_PopupBg] = ImColor(7, 8, 18, 127);

    s.ChildRounding = 0.f;
    s.WindowRounding = 12.f;
    s.WindowPadding = ImVec2(0, 0);

    s.Colors[ImGuiCol_ChildBg] = ImColor(0, 0, 0, 255);
    s.Colors[ImGuiCol_WindowBg] = ImColor(7, 7, 7, 255);
}

namespace Menu {
    void Initialize() {
        LOG_SUCCESS("Menu initialized");
    }

    void Shutdown() {
        LOG_INFO("Menu shutdown");
    }

    void RenderWatermark() {
    }

    void Render() {
        if (!Globals::g_MenuOpen) return;
        if (!Inter_S) return;

        ImGuiStyle* style = &ImGui::GetStyle();
        CustomStyleColor();

        ImGui::SetNextWindowSize(ImVec2(1050, 650));
        ImGui::Begin("General", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
        {
            const auto& p = ImGui::GetWindowPos();
            const ImVec2& region = ImGui::GetContentRegionMax();

            tab_alpha = ImLerp(tab_alpha, (page == active_tab) ? 1.f : 0.f, 18.f * ImGui::GetIO().DeltaTime);
            if (tab_alpha < 0.01f && tab_add < 0.01f) active_tab = page;

            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 250, p.y + region.y), ImGui::GetColorU32(c::child_rect), 12.f, ImDrawFlags_RoundCornersLeft);

            ImGui::GetWindowDrawList()->AddShadowCircle(ImVec2(p.x + 45, p.y + 50), 60.f, ImGui::GetColorU32(c::main_color), 170.f, ImVec2(0, 0));
            ImGui::GetWindowDrawList()->AddShadowCircle(ImVec2(p.x + region.x - 45, p.y + region.y - 45), 60.f, ImGui::GetColorU32(c::main_color), 170.f, ImVec2(0, 0));

            ImGui::GetWindowDrawList()->AddText(Inter_B, 34.f, ImVec2(p.x + 42, p.y + 32), ImGui::GetColorU32(c::main_color), "SPEX");
            ImGui::GetWindowDrawList()->AddText(Inter_B, 34.f, ImVec2(p.x + 120, p.y + 32), ImGui::GetColorU32(c::text_active), "CORE");

            ImGui::SetCursorPos(ImVec2(8, 112));
            ImGui::BeginGroup();
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 8));

                if (gui->tab("AimBot", ICON_FA_CROSSHAIRS, 0 == page, ImVec2(234, 50))) page = 0;      
                if (gui->tab("LegitBot", ICON_FA_BULLSEYE, 1 == page, ImVec2(234, 50))) page = 1;    
                if (gui->tab("Visuals", ICON_FA_EYE, 2 == page, ImVec2(234, 50))) page = 2;     
                if (gui->tab("Settings", ICON_FA_COG, 3 == page, ImVec2(234, 50))) page = 3;    
                if (gui->tab("Config", ICON_FA_FOLDER, 4 == page, ImVec2(234, 50))) page = 4;  

                ImGui::PopStyleVar();
            }
            ImGui::EndGroup();

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * style->Alpha);
            {
                anim_text = ImLerp(anim_text, page == active_tab ? 20.f : 0.f, 14.f * ImGui::GetIO().DeltaTime);

                if (active_tab == 0) {
                    ImGui::GetWindowDrawList()->AddText(Inter_S_2, 23.f, ImVec2(p.x + 246 + anim_text, p.y + 18), ImGui::GetColorU32(c::text_active), "[Aimbot]");

                    ImGui::SetCursorPos(ImVec2(266, 76));
                    ImGui::BeginChild("tab1", ImVec2(376, 154), false);
                    {
                        static int select2 = 0;
                        const char* items2[4]{ "Left Alt", "Right Alt", "Left Shift","Right Shift" };
                        ImGui::Combo("Type", &select2, items2, IM_ARRAYSIZE(items2), 2);
                        ImGui::Checkbox("Enabled", &Globals::Features::g_Aimbot);
                    }
                    ImGui::EndChild();

                    ImGui::SetCursorPos(ImVec2(266, 246));
                    ImGui::BeginChild("tab2", ImVec2(376, 276), false);
                    {
                        ImGui::Checkbox("Through Walls", &Globals::Features::g_Wallhack);
                        static bool BulletTracer = true;
                        ImGui::Checkbox("Bullet Tracer", &BulletTracer);
                        static float color[4] = { 0 / 255.f, 149 / 255.f, 255 / 255.f, 1.0f };
                        ImGui::ColorEdit4("Bullet Color", color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                        static int slider_int = 60;
                        ImGui::SliderInt("I-G Radar Distance", &slider_int, 1, 240, "%du");
                    }
                    ImGui::EndChild();

                    ImGui::SetCursorPos(ImVec2(658, 76));
                    ImGui::BeginChild("tab3", ImVec2(376, 281), false);
                    {
                        static bool Chams = true;
                        ImGui::Checkbox("Enabled Chams", &Chams);
                        static int select3 = 0;
                        const char* items3[4]{ "Glow Outline", "Right Alt", "Left Shift","Right Shift" };
                        ImGui::Combo("Type", &select3, items3, IM_ARRAYSIZE(items3), 2);
                        static float color1[4] = { 255 / 255.f, 0 / 255.f, 0 / 255.f, 1.0f };
                        ImGui::ColorEdit4("Color", color1, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                        static int slider_int1 = 60;
                        ImGui::SliderInt("Distance", &slider_int1, 1, 240, "%ds");
                    }
                    ImGui::EndChild();

                    ImGui::SetCursorPos(ImVec2(658, 373));
                    ImGui::BeginChild("tab4", ImVec2(376, 220), false);
                    {
                        ImGui::Checkbox("Offscreen ESP", &Globals::Features::g_ESP);
                        static bool DynamicBoxes = true;
                        ImGui::Checkbox("Dynamic Boxes", &DynamicBoxes);
                        static float color2[4] = { 0 / 255.f, 255 / 255.f, 55 / 255.f, 1.0f };
                        ImGui::ColorEdit4("Dynamic Boxes Color", color2, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                        static bool DormantESP = true;
                        ImGui::Checkbox("Dormant ESP", &DormantESP);
                    }
                    ImGui::EndChild();
                }
            }
            ImGui::PopStyleVar();
        }
        ImGui::End();
    }
}