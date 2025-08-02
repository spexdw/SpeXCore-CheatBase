#include "FontManager.h"
#include "Logger.h"
#include "core/globals.h"
#include "Font.h"
#include "gui/menu.h"
#include <windows.h>
#include <wininet.h>
#include <filesystem>

#pragma comment(lib, "wininet.lib")

std::unordered_map<FontManager::FontKey, ImFont*, FontManager::FontKeyHash> FontManager::m_fonts;
bool FontManager::m_initialized = false;

bool DownloadFont(const std::string& url, const std::string& filepath) {
    HINTERNET hInternet = InternetOpenA("FontDownloader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return false;
    }

    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return false;
    }

    char buffer[1024];
    DWORD bytesRead;
    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        file.write(buffer, bytesRead);
    }

    file.close();
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
    return true;
}

void FontManager::Initialize() {
    if (m_initialized) return;

    std::string documentsPath = std::getenv("USERPROFILE");
    documentsPath += "\\Documents\\SpeXCore\\fonts";
    std::filesystem::create_directories(documentsPath);

    std::string fontAwesomePath = documentsPath + "\\fa-solid-900.ttf";
    if (!std::filesystem::exists(fontAwesomePath)) {
        if (DownloadFont(Globals::FONTAWESOME_URL, fontAwesomePath)) {
        }
        else {
            LOG_ERROR("Failed to download FontAwesome font");
        }
    }

    m_initialized = true;
    LOG_SUCCESS("FontManager initialized");
}

void FontManager::Shutdown() {
    if (!m_initialized) return;

    m_fonts.clear();
    m_initialized = false;
    LOG_INFO("FontManager shutdown");
}

ImFont* FontManager::LoadFont(const std::string& name, float size, const char* fontPath) {
    if (!m_initialized) {
        LOG_ERROR("FontManager not initialized");
        return nullptr;
    }

    FontKey key = { name, size };

    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        return it->second;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = nullptr;

    if (name == "Inter" && size == 17.0f) {
        font = io.Fonts->AddFontFromMemoryTTF(Inter_SemmiBold, sizeof(Inter_SemmiBold), size, NULL, io.Fonts->GetGlyphRangesCyrillic());
    }
    else if (name == "Inter" && size == 18.0f) {
        font = io.Fonts->AddFontFromMemoryTTF(Inter_SemmiBold, sizeof(Inter_SemmiBold), size, NULL, io.Fonts->GetGlyphRangesCyrillic());
    }
    else if (name == "Inter" && size == 23.0f) {
        font = io.Fonts->AddFontFromMemoryTTF(Inter_SemmiBold, sizeof(Inter_SemmiBold), size, NULL, io.Fonts->GetGlyphRangesCyrillic());
    }
    else if (name == "Inter" && size == 15.0f) {
        font = io.Fonts->AddFontFromMemoryTTF(Inter_SemmiBold, sizeof(Inter_SemmiBold), size, NULL, io.Fonts->GetGlyphRangesCyrillic());
    }
    else if (name == "Inter" && size == 34.0f) {
        font = io.Fonts->AddFontFromMemoryTTF(Inter_Bold, sizeof(Inter_Bold), size, NULL, io.Fonts->GetGlyphRangesCyrillic());
    }
    else if (name == "FontAwesome") {
        std::string documentsPath = std::getenv("USERPROFILE");
        std::string fontAwesomePath = documentsPath + "\\Documents\\SpeXCore\\fonts\\fa-solid-900.ttf";

        if (std::filesystem::exists(fontAwesomePath)) {
            static const ImWchar icon_ranges[] = {
                0xf000, 0xf3ff,
                0
            };

            ImFontConfig config;
            config.MergeMode = false;
            config.PixelSnapH = true;

            font = io.Fonts->AddFontFromFileTTF(fontAwesomePath.c_str(), size, &config, icon_ranges);

            if (font) {
            }
            else {
                LOG_ERROR("FontAwesome loading failed");
            }
        }
        else {
            LOG_ERROR("FontAwesome file not found at: " + fontAwesomePath);
        }
    }
    else if (name == "Icon") {
        font = io.Fonts->AddFontFromMemoryTTF(Icon_Pack, sizeof(Icon_Pack), size, NULL, io.Fonts->GetGlyphRangesCyrillic());
    }
    else if (name == "Arrow") {
        font = io.Fonts->AddFontFromMemoryTTF(Arrow, sizeof(Arrow), size, NULL, io.Fonts->GetGlyphRangesCyrillic());
    }
    else {
        font = io.Fonts->AddFontDefault();
    }

    if (font) {
        m_fonts[key] = font;
        LOG_SUCCESS("Font loaded: " + name + " (" + std::to_string(size) + "px)");
    }
    else {
        LOG_ERROR("Failed to load font: " + name);
    }

    return font;
}

ImFont* FontManager::GetFont(const std::string& name, float size) {
    FontKey key = { name, size };

    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        return it->second;
    }

    return LoadFont(name, size);
}

void FontManager::LoadFontsToImGui() {
    ImGuiIO& io = ImGui::GetIO();

    Inter_S = LoadFont("Inter", 17.0f);
    Inter_S_1 = LoadFont("Inter", 18.0f);
    Inter_S_2 = LoadFont("Inter", 23.0f);
    Inter_S_3 = LoadFont("Inter", 15.0f);
    Inter_B = LoadFont("Inter", 34.0f);
    Icon = LoadFont("FontAwesome", 20.0f);
    Icon_Arrow = LoadFont("Arrow", 6.0f);

    if (Inter_S) {
        io.FontDefault = Inter_S;
    }

    LOG_SUCCESS("All fonts loaded to ImGui");
}