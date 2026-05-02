#pragma once

#include "Character.h"
#include <vector>

// Manages loading and lifetime of all Character assets.
// Assets are sourced from assets/pus/, assets/scissors/, assets/urbino/
// relative to the working directory.
class CharacterManager
{
public:
    CharacterManager();
    ~CharacterManager();

    // Scan asset directories and upload textures to the GPU.
    // Must be called after an OpenGL context is current.
    void LoadAssets();

    // Free all GPU textures, clear the list, then reload from disk.
    void ReloadAssets();

    std::vector<Character>&       GetCharacters()       { return m_characters; }
    const std::vector<Character>& GetCharacters() const { return m_characters; }

private:
    void ScanDirectory(const std::string& dirPath,
                       const std::string& category);
    void FreeAllTextures();

    std::vector<Character> m_characters;
};
