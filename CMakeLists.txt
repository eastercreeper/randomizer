cmake_minimum_required(VERSION 3.20)
project(CharacterRandomizer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_USE_STATIC_MSVC_RUNTIME ON CACHE BOOL "" FORCE)

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
    GIT_TAG        f75e8d1cad7d90d72ef7a4661f1b994ef78b4e31 # 2024-06-08
)
FetchContent_MakeAvailable(stb)

# ── Embed-assets option ───────────────────────────────────────────────────────
# When ON (the default), all images under assets/ are compiled directly into
# the executable so the binary runs without an external assets/ folder.
# Set -DEMBED_ASSETS=OFF to revert to the runtime copy-assets behaviour.
option(EMBED_ASSETS "Compile image assets into the executable at build time" ON)

# ── Sources ───────────────────────────────────────────────────────────────────
set(APP_SOURCES
    src/main.cpp
    src/stb_image_impl.cpp
    src/TextureLoader.cpp
    src/CharacterManager.cpp
    src/Randomizer.cpp
    src/AppUI.cpp
    src/ConfigManager.cpp

)

# ── Windows application icon ──────────────────────────────────────────────────
if(WIN32)
    list(APPEND APP_SOURCES app.rc)
endif()

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# ── Embedded-asset code generation ───────────────────────────────────────────
if(EMBED_ASSETS)
    set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
    file(MAKE_DIRECTORY "${GENERATED_DIR}")

    set(GENERATED_HEADER "${GENERATED_DIR}/EmbeddedAssets.h")
    set(GENERATED_SOURCE "${GENERATED_DIR}/EmbeddedAssets.cpp")

    # Collect all image files so CMake can track them as build dependencies.
    file(GLOB_RECURSE ASSET_FILES
        "${CMAKE_SOURCE_DIR}/assets/pus/*"
        "${CMAKE_SOURCE_DIR}/assets/scissors/*"
        "${CMAKE_SOURCE_DIR}/assets/urbino/*"
        "${CMAKE_SOURCE_DIR}/assets/font/*"
    )
    list(FILTER ASSET_FILES EXCLUDE REGEX "\\.gitkeep$")

    # Generate at configure time so the files exist before the first build.
    execute_process(
        COMMAND ${CMAKE_COMMAND}
            -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
            -DOUTPUT_DIR=${GENERATED_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/GenerateEmbeddedAssets.cmake
        RESULT_VARIABLE GEN_RESULT
    )
    if(NOT GEN_RESULT EQUAL 0)
        message(FATAL_ERROR "GenerateEmbeddedAssets.cmake failed (exit ${GEN_RESULT})")
    endif()

    # Re-generate at build time whenever any asset file (or the script) changes.
    add_custom_command(
        OUTPUT  "${GENERATED_SOURCE}" "${GENERATED_HEADER}"
        COMMAND ${CMAKE_COMMAND}
            -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
            -DOUTPUT_DIR=${GENERATED_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/GenerateEmbeddedAssets.cmake
        DEPENDS ${ASSET_FILES}
                "${CMAKE_SOURCE_DIR}/cmake/GenerateEmbeddedAssets.cmake"
        COMMENT "Regenerating embedded asset sources"
    )

    list(APPEND APP_SOURCES "${GENERATED_SOURCE}")
endif()

add_executable(CharacterRandomizer WIN32 ${APP_SOURCES} ${IMGUI_SOURCES})

target_include_directories(CharacterRandomizer PRIVATE
    src/
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${stb_SOURCE_DIR}
)

if(EMBED_ASSETS)
    target_include_directories(CharacterRandomizer PRIVATE "${GENERATED_DIR}")
    target_compile_definitions(CharacterRandomizer PRIVATE EMBED_ASSETS=1)
endif()

find_package(OpenGL REQUIRED)

target_link_libraries(CharacterRandomizer PRIVATE
    glfw
    OpenGL::GL
)

# ── MinGW: static runtime → single portable .exe (no libgcc/libstdc++ DLLs) ──
# Applied only when building with GCC/MinGW on Windows so MSVC builds are
# unaffected.  -static-libgcc and -static-libstdc++ fold the GCC and C++
# runtimes into the binary.  The Wl,-Bstatic/-Bdynamic pair links winpthread
# statically while still allowing other system libraries to be shared.
if(WIN32 AND MINGW)
    target_link_options(CharacterRandomizer PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
    )
endif()

# ── Copy assets to binary directory (only when NOT embedding) ─────────────────
if(NOT EMBED_ASSETS)
    add_custom_command(TARGET CharacterRandomizer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:CharacterRandomizer>/assets"
        COMMENT "Copying assets to output directory"

            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/assets/font/comic.ttf"
            "$<TARGET_FILE_DIR:CharacterRandomizer>/assets/font/comic.ttf"
    )
endif()

# ── Set debugger working directory (MSVC / CLion CMake) ──────────────────────
set_target_properties(CharacterRandomizer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:CharacterRandomizer>"
)
