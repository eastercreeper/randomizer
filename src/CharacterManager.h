#pragma once

#include "Character.h"
#include <vector>

// Manages loading and lifetime of all Character assets.
//
// When built with EMBED_ASSETS=1 (default), assets are compiled into the
// binary via the generated EmbeddedAssets.h/.cpp and loaded with
// LoadTextureFromMemory – no external assets/ folder is required.
//
// When built without EMBED_ASSETS, assets are read from
// assets/pus/, assets/scissors/, assets/urbino/ relative to the working
// directory (legacy behaviour).
class CharacterManager
{
public:
    CharacterManager();
    ~CharacterManager();

    // Load all assets and upload textures to the GPU.
    // Must be called after an OpenGL context is current.
    void LoadAssets();

    // Free all GPU textures, clear the list, then reload.
    void ReloadAssets();

    std::vector<Character>&       GetCharacters()       { return m_characters; }
    const std::vector<Character>& GetCharacters() const { return m_characters; }
    void FreeAllTextures();

    std::vector<Character> m_characters;

private:
#ifdef EMBED_ASSETS
    void LoadEmbeddedAssets();
#else
    void ScanDirectory(const std::string& dirPath,
                       const std::string& category);
#endif
};
