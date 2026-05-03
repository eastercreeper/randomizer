# CharacterRandomizer

A C++20 GUI application built with **Dear ImGui + GLFW + OpenGL 3.3**.

## Features

- All character images are **embedded directly in the executable** at build time – no external `assets/` folder is needed to run the app.
- Displays every image as a clickable thumbnail; click to **enable / disable** that character
- **Category filter checkboxes** (Pus / Scissors / Urbino)
- **Randomize** button – picks one random character from the enabled + filtered pool and shows it enlarged with its name and category
- **Randomize Attack/Defense** button – picks one defender (Pus/Urbino) and one attacker (Scissors/Urbino)
- **Reload Assets** button – re-uploads embedded textures to the GPU (useful after a context reset)

Supported image formats: PNG, JPG, JPEG, BMP, TGA (via stb_image).

---

## Building in CLion (Windows)

### Prerequisites

| Tool | Notes |
|------|-------|
| CMake ≥ 3.20 | bundled with CLion |
| Git | required by FetchContent to clone GLFW / ImGui / stb |
| C++ compiler | MSVC (Visual Studio Build Tools) or MinGW-w64 (CLion bundled) |
| OpenGL driver | any modern GPU driver |

### Steps

1. **Open** the repository folder in CLion (`File → Open`).
2. CLion will detect `CMakeLists.txt` and start CMake configuration automatically.  
   FetchContent will download GLFW, Dear ImGui, and stb on the first run – this may take a minute.  
   The asset-embedding step also runs at configure time and generates `build/generated/EmbeddedAssets.cpp`.
3. Select the `CharacterRandomizer` run target and click **Run**.

The resulting `CharacterRandomizer.exe` is fully self-contained – you can copy it anywhere and run it without an `assets/` folder.

> **Tip:** To build without embedding (external assets folder mode), pass `-DEMBED_ASSETS=OFF` to CMake:  
> `cmake -S . -B build -DEMBED_ASSETS=OFF`

### Portable single-exe build (MinGW / CLion bundled toolchain)

If you use CLion's bundled MinGW toolchain, the build automatically links the GCC
runtime (`libgcc_s_seh-1.dll`), C++ standard library (`libstdc++-6.dll`), and
winpthread (`libwinpthread-1.dll`) **statically** into the executable.  The
resulting `.exe` is therefore self-contained and runs on a clean Windows machine
without needing to ship those DLLs alongside it.

This is handled in `CMakeLists.txt` via the following (MinGW-only) linker options:

```cmake
target_link_options(CharacterRandomizer PRIVATE
    -static-libgcc
    -static-libstdc++
    -Wl,-Bstatic,-lwinpthread,-Bdynamic
)
```

These flags are guarded by `if(WIN32 AND MINGW)` so MSVC builds are completely unaffected.

#### Building a Release exe from CLion

1. Go to **Settings → Build, Execution, Deployment → CMake**.
2. Add a **Release** profile:
   - **Build type:** `Release`
   - **Generation path:** `cmake-build-release`
3. Select the **Release** profile in the top-right toolbar.
4. Click the **Build** (hammer) button or use **Build → Build Project**.
5. The final portable executable is at:
   ```
   cmake-build-release/CharacterRandomizer.exe
   ```
   Copy it anywhere on any Windows machine – no extra DLLs or folders required.

### Adding or replacing character images

1. Drop image files (`.png`, `.jpg`, `.jpeg`, `.bmp`, `.tga`) into the appropriate folder:

   ```
   assets/
     pus/        ← pus characters
     scissors/   ← scissors characters
     urbino/     ← urbino characters
   ```

2. **Rebuild** the project – CMake detects the changed asset files and automatically regenerates the embedded C++ source before compiling.  
   You do *not* need to manually run any extra tool.

> **Note:** With `EMBED_ASSETS=ON` (the default), the **Reload Assets** button re-uploads the already-compiled texture data from memory; it does not re-scan a folder on disk.

---

## Project structure

```
CharacterRandomizer/
├── CMakeLists.txt           – build script; fetches GLFW, ImGui, stb; embeds assets
├── app.rc                   – Windows resource script (embeds icon.ico into the exe)
├── icon.ico                 – application icon (Windows)
├── cmake/
│   └── GenerateEmbeddedAssets.cmake  – generates EmbeddedAssets.h/.cpp from assets/
├── assets/
│   ├── pus/
│   ├── scissors/
│   └── urbino/
└── src/
    ├── main.cpp             – window creation, OpenGL context, main loop
    ├── Character.h          – POD struct (name, category, textureId, enabled…)
    ├── TextureLoader.h/.cpp – stb_image → OpenGL texture upload/free (file + memory)
    ├── CharacterManager.h/.cpp – loads Character list from embedded assets or disk
    ├── Randomizer.h/.cpp    – random pick from a filtered pool
    ├── AppUI.h/.cpp         – all Dear ImGui rendering
    └── stb_image_impl.cpp   – single TU that defines STB_IMAGE_IMPLEMENTATION
```
