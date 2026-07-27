#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <Windows.h>

// Include GLFW after the imgui backends so GLFW_INCLUDE_NONE is not needed –
// the ImGui OpenGL3 backend uses its own bundled loader for modern GL calls,
// while we only use the GL 1.x functions (viewport, clear) available from the
// system OpenGL headers that GLFW drags in automatically.
#include <GLFW/glfw3.h>

#include "CharacterManager.h"
#include "Randomizer.h"
#include "AppUI.h"
#include "ConfigManager.h"

#include <algorithm>
#include <cstdio>

static void GlfwErrorCallback(int error, const char* description)
{
    std::fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main()
{
    glfwSetErrorCallback(GlfwErrorCallback);
    if (!glfwInit())
        return 1;

    // Request OpenGL 3.3 core profile (works on any modern GPU / driver)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        1280, 800, "CharacterRandomizer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // ── Dear ImGui setup ──────────────────────────────────────────────────
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFont* comicFont = io.Fonts->AddFontFromFileTTF(
        "assets/font/comic.ttf",
        18.0f
    );

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // ── Application logic (created after OpenGL context is ready) ─────────
    CharacterManager mgr;
    mgr.LoadAssets();

    ConfigManager::Instance().load();

    for (auto& ch : mgr.GetCharacters())
    {
        ch.enabled = ConfigManager::Instance().Get(ch.name, true);
    }

    // Auto-size window width based on loaded character count, capped at 5 columns.
    {
        constexpr int kThumbSize   = 80;
        constexpr int kCellWidth   = kThumbSize + 12;
        constexpr int kSidePadding = 40;
        const int characterCount = static_cast<int>(mgr.GetCharacters().size());
        const int cols = std::clamp(characterCount, 1, 9);
        const int computedWidth = cols * kCellWidth + kSidePadding;
        glfwSetWindowSize(window, computedWidth, 800);
    }

    Randomizer  rng;
    AppUI       ui(mgr, rng);

    // ── Main loop ─────────────────────────────────────────────────────────
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ui.Render();

        ImGui::Render();

        int displayW = 0, displayH = 0;
        glfwGetFramebufferSize(window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // ── Cleanup ───────────────────────────────────────────────────────────
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return main();
}