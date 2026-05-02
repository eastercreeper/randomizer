#pragma once

#include <string>

// Loads an image from disk and uploads it as an OpenGL 2D texture.
// Returns the GL texture ID (non-zero on success, 0 on failure).
// outWidth / outHeight receive the image dimensions in pixels.
unsigned int LoadTextureFromFile(const std::string& path,
                                 int& outWidth,
                                 int& outHeight);

// Deletes a previously created GL texture.
void FreeTexture(unsigned int textureId);
