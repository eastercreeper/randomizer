# CharacterRandomizer

A C++20 GUI application built with **Dear ImGui + GLFW + OpenGL 3.3**.

## Features

- Loads character images from `assets/pus/`, `assets/scissors/`, `assets/urbino/`
- Displays every image as a clickable thumbnail; click to **enable / disable** that character
- **Category filter checkboxes** (Pus / Scissors / Urbino)
- **Randomize** button – picks one random character from the enabled + filtered pool and shows it enlarged with its name and category
- **Reload Assets** button – rescans all asset folders and re-uploads textures

Supported image formats: PNG, JPG, JPEG, BMP, TGA (via stb_image).

---

## Building in CLion (Windows)

### Prerequisites

| Tool | Notes |
|------|-------|
| CMake ≥ 3.20 | bundled with CLion |
| Git | required by FetchContent to clone GLFW / ImGui / stb |
| C++ compiler | MSVC (Visual Studio Build Tools) or MinGW-w64 |
| OpenGL driver | any modern GPU driver |

### Steps

1. **Open** the repository folder in CLion (`File → Open`).
2. CLion will detect `CMakeLists.txt` and start CMake configuration automatically.  
   FetchContent will download GLFW, Dear ImGui, and stb on the first run – this may take a minute.
3. Select the `CharacterRandomizer` run target and click **Run**.

> **Working directory** – the `POST_BUILD` step copies `assets/` next to the executable, so the binary can always find its images regardless of build location.  
> CLion also sets the debugger working directory to the output folder via `VS_DEBUGGER_WORKING_DIRECTORY`.

### Adding characters

Drop image files (`.png`, `.jpg`, `.jpeg`, `.bmp`, `.tga`) into the matching folder:

```
assets/
  pus/        ← pus characters
  scissors/   ← scissors characters
  urbino/     ← urbino characters
```

Then press **Reload Assets** inside the app, or rebuild to trigger the copy step again.

---

## Project structure

```
CharacterRandomizer/
├── CMakeLists.txt           – build script; fetches GLFW, ImGui, stb
├── assets/
│   ├── pus/
│   ├── scissors/
│   └── urbino/
└── src/
    ├── main.cpp             – window creation, OpenGL context, main loop
    ├── Character.h          – POD struct (name, category, textureId, enabled…)
    ├── TextureLoader.h/.cpp – stb_image → OpenGL texture upload/free
    ├── CharacterManager.h/.cpp – scans asset dirs, owns Character list
    ├── Randomizer.h/.cpp    – random pick from a filtered pool
    ├── AppUI.h/.cpp         – all Dear ImGui rendering
    └── stb_image_impl.cpp   – single TU that defines STB_IMAGE_IMPLEMENTATION
```
