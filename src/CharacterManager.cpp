#include "CharacterManager.h"

#include <cstring>

#include "TextureLoader.h"

#ifdef EMBED_ASSETS
#  include "EmbeddedAssets.h"
#else
#  include <filesystem>
#  include <algorithm>
#  include <array>

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
#endif // !EMBED_ASSETS

CharacterManager::CharacterManager() = default;

CharacterManager::~CharacterManager()
{
    FreeAllTextures();
}

void CharacterManager::LoadAssets()
{
#ifdef EMBED_ASSETS
    LoadEmbeddedAssets();
#else
    for (const auto& cat : kCategories)
        ScanDirectory("assets/" + std::string(cat), std::string(cat));
#endif
}

void CharacterManager::ReloadAssets()
{
    FreeAllTextures();
    m_characters.clear();
    LoadAssets();
}

#ifdef EMBED_ASSETS

void CharacterManager::LoadEmbeddedAssets()
{
    // kEmbeddedAssets is already sorted alphabetically within each category
    // (the generator script sorts file paths before emitting them).
    for (std::size_t i = 0; i < kEmbeddedAssetCount; ++i) {
        const EmbeddedAsset& ea = kEmbeddedAssets[i];

        // Ignore non-character assets
        if (std::strcmp(ea.category, "font") == 0)
            continue;

        Character ch;
        ch.name      = ea.name;
        ch.category  = ea.category;
        ch.filepath  = "(embedded)";
        ch.enabled   = true;
        ch.textureId = LoadTextureFromMemory(ea.data, ea.size,
                                             ch.width, ch.height);
        m_characters.push_back(std::move(ch));
    }
}

#else // filesystem path

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

#endif // EMBED_ASSETS

void CharacterManager::FreeAllTextures()
{
    for (auto& ch : m_characters)
        FreeTexture(ch.textureId);
}
