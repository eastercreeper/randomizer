#include "CharacterManager.h"
#include "TextureLoader.h"

#include <filesystem>
#include <algorithm>
#include <array>

namespace fs = std::filesystem;

static constexpr std::array<std::string_view, 3> kCategories = {
    "pus", "scissors", "urbino"
};

static constexpr std::array<std::string_view, 5> kImageExts = {
    ".png", ".jpg", ".jpeg", ".bmp", ".tga"
};

static bool IsImageFile(const fs::path& p)
{
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    for (const auto& e : kImageExts)
        if (ext == e) return true;
    return false;
}

CharacterManager::CharacterManager() = default;

CharacterManager::~CharacterManager()
{
    FreeAllTextures();
}

void CharacterManager::LoadAssets()
{
    for (const auto& cat : kCategories)
        ScanDirectory("assets/" + std::string(cat), std::string(cat));
}

void CharacterManager::ReloadAssets()
{
    FreeAllTextures();
    m_characters.clear();
    LoadAssets();
}

void CharacterManager::ScanDirectory(const std::string& dirPath,
                                      const std::string& category)
{
    fs::path dir(dirPath);
    if (!fs::exists(dir) || !fs::is_directory(dir))
        return;

    std::vector<fs::path> paths;
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file() && IsImageFile(entry.path()))
            paths.push_back(entry.path());
    }
    // Sort for stable, alphabetical ordering
    std::sort(paths.begin(), paths.end());

    for (const auto& p : paths) {
        Character ch;
        ch.name     = p.stem().string();
        ch.category = category;
        ch.filepath = p.string();
        ch.enabled  = true;
        ch.textureId = LoadTextureFromFile(ch.filepath, ch.width, ch.height);
        m_characters.push_back(std::move(ch));
    }
}

void CharacterManager::FreeAllTextures()
{
    for (auto& ch : m_characters)
        FreeTexture(ch.textureId);
}
