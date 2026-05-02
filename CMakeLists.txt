cmake_minimum_required(VERSION 3.20)
project(CharacterRandomizer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ── GLFW ──────────────────────────────────────────────────────────────────────
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.9
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ── Dear ImGui ────────────────────────────────────────────────────────────────
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.90.4
)
FetchContent_MakeAvailable(imgui)

# ── stb ───────────────────────────────────────────────────────────────────────
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# ── Sources ───────────────────────────────────────────────────────────────────
set(APP_SOURCES
    src/main.cpp
    src/stb_image_impl.cpp
    src/TextureLoader.cpp
    src/CharacterManager.cpp
    src/Randomizer.cpp
    src/AppUI.cpp
)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(CharacterRandomizer ${APP_SOURCES} ${IMGUI_SOURCES})

target_include_directories(CharacterRandomizer PRIVATE
    src/
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${stb_SOURCE_DIR}
)

find_package(OpenGL REQUIRED)

target_link_libraries(CharacterRandomizer PRIVATE
    glfw
    OpenGL::GL
)

# ── Copy assets to binary directory on every build ───────────────────────────
add_custom_command(TARGET CharacterRandomizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:CharacterRandomizer>/assets"
    COMMENT "Copying assets to output directory"
)

# ── Set debugger working directory (MSVC / CLion CMake) ──────────────────────
set_target_properties(CharacterRandomizer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:CharacterRandomizer>"
)
