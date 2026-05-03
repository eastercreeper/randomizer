#include "AppUI.h"

#include <imgui.h>

#include <algorithm>
#include <string>
#include <vector>

// Returns a display size that fits `imgW x imgH` within a `boxSize x boxSize`
// square while preserving the original aspect ratio.
static ImVec2 FitInBox(int imgW, int imgH, float boxSize)
{
    if (imgW <= 0 || imgH <= 0)
        return ImVec2(boxSize, boxSize);
    const float aspect = static_cast<float>(imgW) / static_cast<float>(imgH);
    if (aspect >= 1.f)
        return ImVec2(boxSize, boxSize / aspect);
    else
        return ImVec2(boxSize * aspect, boxSize);
}

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
        m_selectedAttack  = nullptr;
        m_selectedDefense = nullptr;
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
    if (ImGui::Button("Randomize Attack/Defense")) {
        m_selected = nullptr;
        std::vector<const Character*> defensePool;
        std::vector<const Character*> attackPool;
        for (const auto& ch : m_mgr.GetCharacters()) {
            if (!ch.enabled) continue;
            if (ch.category == "pus"     || ch.category == "urbino")   defensePool.push_back(&ch);
            if (ch.category == "scissors"|| ch.category == "urbino")   attackPool.push_back(&ch);
        }
        m_selectedDefense = m_rng.Pick(defensePool);
        m_selectedAttack  = m_rng.Pick(attackPool);
    }

    ImGui::SameLine();
    if (ImGui::Button("Reload Assets")) {
        m_selected        = nullptr;
        m_selectedAttack  = nullptr;
        m_selectedDefense = nullptr;
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

    const ImGuiStyle& style = ImGui::GetStyle();
    const float cellW   = kThumbSize + style.FramePadding.x * 2.f + style.ItemSpacing.x;
    const float avail   = ImGui::GetContentRegionAvail().x;
    const int   columns = std::max(1, static_cast<int>((avail + style.ItemSpacing.x) / cellW));

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

        // Highlight the currently selected character (single or attack/defense)
        const bool isSelected = (m_selected == &ch)
                             || (m_selectedAttack  == &ch)
                             || (m_selectedDefense == &ch);
        if (isSelected)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.55f, 1.f, 1.f));

        const std::string btnId = "##ch_" + ch.category + "_" + ch.name;
        bool clicked = false;

        if (ch.textureId != 0) {
            const ImVec2 displaySize = FitInBox(ch.width, ch.height, kThumbSize);
            // Pad FramePadding so the outer button is always kThumbSize×kThumbSize,
            // keeping every cell the same width regardless of image aspect ratio.
            const float padX = (kThumbSize - displaySize.x) * 0.5f + style.FramePadding.x;
            const float padY = (kThumbSize - displaySize.y) * 0.5f + style.FramePadding.y;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padX, padY));
            clicked = ImGui::ImageButton(
                btnId.c_str(),
                reinterpret_cast<ImTextureID>(
                    static_cast<uintptr_t>(ch.textureId)),
                displaySize,
                ImVec2(0.f, 0.f), ImVec2(1.f, 1.f),
                ImVec4(0.f, 0.f, 0.f, 0.f),
                tint);
            ImGui::PopStyleVar();
        } else {
            // Fallback when texture failed to load – btnId starts with "##"
            // so ImGui hides it; prefix the visible name separately.
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

    // ── Single-pick result ────────────────────────────────────────────────────
    if (m_selected) {
        if (m_selected->textureId != 0) {
            const ImVec2 displaySize =
                FitInBox(m_selected->width, m_selected->height, kResultSize);
            ImGui::Image(
                reinterpret_cast<ImTextureID>(
                    static_cast<uintptr_t>(m_selected->textureId)),
                displaySize);
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
        return;
    }

    // ── Attack / Defense result ───────────────────────────────────────────────
    if (m_selectedAttack || m_selectedDefense) {
        auto renderSide = [&](const char* label, const Character* ch) {
            ImGui::Text("%s:", label);
            if (!ch) {
                ImGui::TextDisabled("  (none available)");
                return;
            }
            if (ch->textureId != 0) {
                const ImVec2 displaySize =
                    FitInBox(ch->width, ch->height, kResultSize);
                ImGui::Image(
                    reinterpret_cast<ImTextureID>(
                        static_cast<uintptr_t>(ch->textureId)),
                    displaySize);
                ImGui::SameLine();
            }
            ImGui::BeginGroup();
            ImGui::Spacing();
            ImGui::Text("Name:     %s", ch->name.c_str());
            ImGui::Text("Category: %s", ch->category.c_str());
            ImGui::Text("File:     %s", ch->filepath.c_str());
            if (ch->width > 0)
                ImGui::Text("Size:     %d x %d px", ch->width, ch->height);
            ImGui::EndGroup();
        };

        renderSide("Defense (Pus / Urbino)",       m_selectedDefense);
        ImGui::Spacing();
        renderSide("Attack  (Scissors / Urbino)",  m_selectedAttack);
        return;
    }

    ImGui::TextDisabled("Press 'Randomize' to pick a character.");
}
