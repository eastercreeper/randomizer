#pragma once

#include <cstddef>
#include <string>

// Loads an image from disk and uploads it as an OpenGL 2D texture.
// Returns the GL texture ID (non-zero on success, 0 on failure).
// outWidth / outHeight receive the image dimensions in pixels.
unsigned int LoadTextureFromFile(const std::string& path,
                                 int& outWidth,
                                 int& outHeight);

// Loads an image from an in-memory buffer (e.g. an embedded asset) and uploads
// it as an OpenGL 2D texture.  Uses stbi_load_from_memory internally.
// Returns the GL texture ID (non-zero on success, 0 on failure).
unsigned int LoadTextureFromMemory(const unsigned char* data,
                                   std::size_t          size,
                                   int&                 outWidth,
                                   int&                 outHeight);

// Deletes a previously created GL texture.
void FreeTexture(unsigned int textureId);
