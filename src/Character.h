#pragma once

#include <string>

// A single character entry loaded from one of the asset folders.
struct Character {
    std::string  name;       // filename without extension
    std::string  category;   // "pus" | "scissors" | "urbino"
    std::string  filepath;   // absolute or relative path used to load the image
    unsigned int textureId = 0;
    int          width     = 0;
    int          height    = 0;
    bool         enabled   = true;
};
