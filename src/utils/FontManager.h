#pragma once
#include "imgui.h"
#include <string>
#include <unordered_map>

class FontManager {
public:
    static void Initialize();
    static void Shutdown();

    static ImFont* LoadFont(const std::string& name, float size, const char* fontPath = nullptr);
    static ImFont* GetFont(const std::string& name, float size);
    static void LoadFontsToImGui();

private:
    struct FontKey {
        std::string name;
        float size;

        bool operator==(const FontKey& other) const {
            return name == other.name && size == other.size;
        }
    };

    struct FontKeyHash {
        std::size_t operator()(const FontKey& key) const {
            return std::hash<std::string>{}(key.name) ^ std::hash<float>{}(key.size);
        }
    };

    static std::unordered_map<FontKey, ImFont*, FontKeyHash> m_fonts;
    static bool m_initialized;
};