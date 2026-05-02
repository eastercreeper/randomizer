#include "AppUI.h"

#include <imgui.h>

#include <algorithm>
#include <string>
#include <vector>

AppUI::AppUI(CharacterManager& mgr, Randomizer& rng)
    : m_mgr(mgr), m_rng(rng)
{}

void AppUI::Render()
{
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("CharacterRandomizer", nullptr,
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove   |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoTitleBar);

    RenderFilterBar();
    ImGui::Separator();
    RenderCharacterGrid();
    ImGui::Separator();
    RenderResultPanel();

    ImGui::End();
}

// ── Filter bar ────────────────────────────────────────────────────────────────

void AppUI::RenderFilterBar()
{
    ImGui::Text("Filter:");
    ImGui::SameLine();
    ImGui::Checkbox("Pus",      &m_filterPus);
    ImGui::SameLine();
    ImGui::Checkbox("Scissors", &m_filterScissors);
    ImGui::SameLine();
    ImGui::Checkbox("Urbino",   &m_filterUrbino);

    ImGui::SameLine(0.f, 20.f);

    if (ImGui::Button("Randomize")) {
        std::vector<const Character*> pool;
        for (const auto& ch : m_mgr.GetCharacters()) {
            if (!ch.enabled)                                         continue;
            if (ch.category == "pus"      && !m_filterPus)      continue;
            if (ch.category == "scissors" && !m_filterScissors) continue;
            if (ch.category == "urbino"   && !m_filterUrbino)   continue;
            pool.push_back(&ch);
        }
        m_selected = m_rng.Pick(pool);
    }

    ImGui::SameLine();
    if (ImGui::Button("Reload Assets")) {
        m_selected = nullptr;
        m_mgr.ReloadAssets();
    }
}

// ── Character grid ────────────────────────────────────────────────────────────

void AppUI::RenderCharacterGrid()
{
    ImGui::Text("Characters  (click thumbnail to enable / disable):");

    auto& chars = m_mgr.GetCharacters();
    if (chars.empty()) {
        ImGui::TextDisabled(
            "No characters loaded. "
            "Add images to assets/pus/, assets/scissors/, assets/urbino/");
        return;
    }

    const float avail   = ImGui::GetContentRegionAvail().x;
    const int   columns = std::max(1, static_cast<int>(avail / (kThumbSize + 12.f)));

    int col = 0;
    for (auto& ch : chars) {
        // Respect active category filters
        if (ch.category == "pus"      && !m_filterPus)      continue;
        if (ch.category == "scissors" && !m_filterScissors) continue;
        if (ch.category == "urbino"   && !m_filterUrbino)   continue;

        if (col > 0 && col % columns != 0)
            ImGui::SameLine();

        // Dimmed tint for disabled characters
        const ImVec4 tint = ch.enabled
            ? ImVec4(1.f, 1.f, 1.f, 1.f)
            : ImVec4(0.3f, 0.3f, 0.3f, 0.7f);

        // Highlight the currently selected character
        const bool isSelected = (m_selected == &ch);
        if (isSelected)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.55f, 1.f, 1.f));

        const std::string btnId = "##ch_" + ch.category + "_" + ch.name;
        bool clicked = false;

        if (ch.textureId != 0) {
            // ImGui 1.89+ ImageButton signature: (str_id, tex_id, size, ...)
            clicked = ImGui::ImageButton(
                btnId.c_str(),
                reinterpret_cast<ImTextureID>(
                    static_cast<uintptr_t>(ch.textureId)),
                ImVec2(kThumbSize, kThumbSize),
                ImVec2(0.f, 0.f), ImVec2(1.f, 1.f),
                ImVec4(0.f, 0.f, 0.f, 0.f),
                tint);
        } else {
            // Fallback when texture failed to load
            clicked = ImGui::Button(
                (ch.name + btnId).c_str(),
                ImVec2(kThumbSize, kThumbSize));
        }

        if (isSelected)
            ImGui::PopStyleColor();

        if (clicked)
            ch.enabled = !ch.enabled;

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s  [%s]", ch.name.c_str(), ch.category.c_str());
            ImGui::Text("%s", ch.enabled ? "Enabled" : "Disabled");
            ImGui::EndTooltip();
        }

        ++col;
    }
}

// ── Result panel ─────────────────────────────────────────────────────────────

void AppUI::RenderResultPanel()
{
    ImGui::Text("Result:");
    if (!m_selected) {
        ImGui::TextDisabled("Press 'Randomize' to pick a character.");
        return;
    }

    if (m_selected->textureId != 0) {
        ImGui::Image(
            reinterpret_cast<ImTextureID>(
                static_cast<uintptr_t>(m_selected->textureId)),
            ImVec2(kResultSize, kResultSize));
        ImGui::SameLine();
    }

    ImGui::BeginGroup();
    ImGui::Spacing();
    ImGui::Text("Name:     %s", m_selected->name.c_str());
    ImGui::Text("Category: %s", m_selected->category.c_str());
    ImGui::Text("File:     %s", m_selected->filepath.c_str());
    if (m_selected->width > 0)
        ImGui::Text("Size:     %d x %d px",
                    m_selected->width, m_selected->height);
    ImGui::EndGroup();
}
