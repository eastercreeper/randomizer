#include "TextureLoader.h"

// Include GLFW which drags in the system OpenGL headers (GL 1.x functions
// needed here – glGenTextures, glTexImage2D, etc. – are all OpenGL 1.1 and
// exported directly from opengl32 without a loader).
#include <GLFW/glfw3.h>

// GL_CLAMP_TO_EDGE was added in OpenGL 1.2; guard for older SDK headers.
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#include <stb_image.h>

// ── Shared helper ─────────────────────────────────────────────────────────────

static unsigned int UploadTexture(unsigned char* pixels,
                                  int width, int height)
{
    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    return static_cast<unsigned int>(texId);
}

// ── Public API ────────────────────────────────────────────────────────────────

unsigned int LoadTextureFromFile(const std::string& path,
                                 int& outWidth,
                                 int& outHeight)
{
    stbi_set_flip_vertically_on_load(false);

    int channels = 0;
    unsigned char* data = stbi_load(path.c_str(),
                                    &outWidth, &outHeight,
                                    &channels, 4 /*force RGBA*/);
    if (!data)
        return 0;

    unsigned int texId = UploadTexture(data, outWidth, outHeight);
    stbi_image_free(data);
    return texId;
}

unsigned int LoadTextureFromMemory(const unsigned char* data,
                                   std::size_t          size,
                                   int&                 outWidth,
                                   int&                 outHeight)
{
    stbi_set_flip_vertically_on_load(false);

    int channels = 0;
    unsigned char* pixels = stbi_load_from_memory(
        data, static_cast<int>(size),
        &outWidth, &outHeight,
        &channels, 4 /*force RGBA*/);
    if (!pixels)
        return 0;

    unsigned int texId = UploadTexture(pixels, outWidth, outHeight);
    stbi_image_free(pixels);
    return texId;
}

void FreeTexture(unsigned int textureId)
{
    if (textureId != 0) {
        GLuint id = static_cast<GLuint>(textureId);
        glDeleteTextures(1, &id);
    }
}
