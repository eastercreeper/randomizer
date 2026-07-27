#include "AppUI.h"
#include "ConfigManager.h"

#include <imgui.h>

#include <algorithm>
#include <array>
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

struct UpgradeCategoryDefinition
{
    const char* name;
    std::array<const char*, 2> upgrades;
};

static const std::vector<std::string> kUtilityOptions = {
    "Frag", "Flashbang", "Slow", "Windstorm", "Heal", "Smoke", "Tattletale", "Shield", "Interceptor"
};

static const std::vector<std::string> kSecondaryOptions = {
    "Stinger", "Athena", "Ignis", "Emberspit"
};

static const std::vector<std::string> kMeleeOptions = {
    "Greatsword", "Ninjato", "Scythe"
};
static const std::vector<std::string> kAwakeningOptions = {
    "Awakening 2", "Awakening 3"
};


static const std::array<UpgradeCategoryDefinition, 4> kWeaponUpgradeCategories = { {
    {"Core"},
    {"Firing"},
    {"Capacity"},
    {"Accuracy/Functions"}
} };

static const std::array<UpgradeCategoryDefinition, 2> kCharacterUpgradeCategories = { {
    {"Armor"},
    {"Stringification"}
} };

static const std::array<UpgradeCategoryDefinition, 2> kAbilityUpgradeCategories = { {
    {"Active Skill"},
    {"Passive Skill"}
} };

// ── App-wide theme ───────────────────────────────────────────────────────────
// One cohesive dark theme + accent color applied to the whole window, so
// every tab shares the same spacing, rounding, and palette instead of each
// widget picking its own look. Counts below must match the Pop* calls.
static constexpr int kThemeVarCount = 10;
static constexpr int kThemeColorCount = 15;

static void PushAppTheme()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18.f, 16.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, 6.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.f, 8.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(8.f, 6.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 8.f);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.f);

    const ImVec4 accent(0.20f, 0.55f, 1.00f, 1.00f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.11f, 0.13f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.13f, 0.14f, 0.16f, 0.60f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.17f, 0.20f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.20f, 0.22f, 0.26f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.22f, 0.24f, 0.29f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.28f, 0.30f, 0.34f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.20f, 0.24f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.27f, 0.32f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, accent);
    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.14f, 0.15f, 0.18f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.24f, 0.50f, 0.90f, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_TabActive, accent);
    ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(0.12f, 0.13f, 0.15f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(0.18f, 0.30f, 0.50f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark, accent);
}

static void PopAppTheme()
{
    ImGui::PopStyleColor(kThemeColorCount);
    ImGui::PopStyleVar(kThemeVarCount);
}

AppUI::AppUI(CharacterManager& mgr, Randomizer& rng)
    : m_mgr(mgr), m_rng(rng)
{
}

void AppUI::Render()
{
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(io.DisplaySize);

    PushAppTheme();
    ImGui::Begin("CharacterRandomizer", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar);

    // Since the native title bar is hidden, give the window a visible
    // heading of its own.
    ImGui::PushFont(gTitleFont);
    ImGui::TextUnformatted("Strinover Randomizer");
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::BeginTabBar("RandomizerTabs")) {
        if (ImGui::BeginTabItem("Roster")) {
            ImGui::Spacing();
            RenderFilterBar();
            ImGui::Separator();
            ImGui::Spacing();
            RenderCharacterGrid();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Loadout")) {
            ImGui::Spacing();
            RenderUpgradeRandomizerTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
    PopAppTheme();
}

// ── Filter bar ────────────────────────────────────────────────────────────────

void AppUI::RenderFilterBar()
{
    ImGui::TextDisabled("FILTER");
    ImGui::SameLine();
    ImGui::Checkbox("Pus", &m_filterPus);
    ImGui::SameLine();
    ImGui::Checkbox("Scissors", &m_filterScissors);
    ImGui::SameLine();
    ImGui::Checkbox("Urbino", &m_filterUrbino);

    ImGui::SameLine(0.f, 24.f);

    if (ImGui::Button("Randomize")) {
        m_selectedAttack = nullptr;
        m_selectedDefense = nullptr;
        std::vector<const Character*> pool;
        for (const auto& ch : m_mgr.GetCharacters()) {
            if (!ch.enabled)                                         continue;
            if (ch.category == "pus" && !m_filterPus)      continue;
            if (ch.category == "scissors" && !m_filterScissors) continue;
            if (ch.category == "urbino" && !m_filterUrbino)   continue;
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
            if (ch.category == "pus" && !m_filterPus)      continue;
            if (ch.category == "scissors" && !m_filterScissors) continue;
            if (ch.category == "urbino" && !m_filterUrbino)   continue;
            if (ch.category == "pus" || ch.category == "urbino")   defensePool.push_back(&ch);
            if (ch.category == "scissors" || ch.category == "urbino")   attackPool.push_back(&ch);
        }
        m_selectedDefense = m_rng.Pick(defensePool);
        m_selectedAttack = m_rng.Pick(attackPool, false);
    }

    ImGui::SameLine();
    if (ImGui::Button("Reload Assets")) {
        m_selected = nullptr;
        m_selectedAttack = nullptr;
        m_selectedDefense = nullptr;
        m_mgr.ReloadAssets();
    }
}

// ── Character grid ────────────────────────────────────────────────────────────
// Note: the standalone results panel has been removed. The current pick(s)
// are still visible via the colored highlight on the matching thumbnail(s)
// below (blue = single pick, red = attack, blue-accent = defense,
// purple = both).

void AppUI::RenderCharacterGrid()
{
    ImGui::TextDisabled("ROSTER");
    ImGui::SameLine();
    ImGui::TextDisabled("— click a thumbnail to enable / disable");
    ImGui::Spacing();

    auto& chars = m_mgr.GetCharacters();
    if (chars.empty()) {
        ImGui::TextDisabled(
            "No characters loaded. "
            "Add images to assets/pus/, assets/scissors/, assets/urbino/");
        return;
    }

    const ImGuiStyle& style = ImGui::GetStyle();
    const float cellW = kThumbSize + style.FramePadding.x * 2.f + style.ItemSpacing.x;
    const float avail = ImGui::GetContentRegionAvail().x;
    const int   columns = std::max(1, static_cast<int>((avail + style.ItemSpacing.x) / cellW));

    int col = 0;
    for (auto& ch : chars) {
        // Respect active category filters
        if (ch.category == "pus" && !m_filterPus)      continue;
        if (ch.category == "scissors" && !m_filterScissors) continue;
        if (ch.category == "urbino" && !m_filterUrbino)   continue;

        if (col > 0 && col % columns != 0)
            ImGui::SameLine();

        // Dimmed tint for disabled characters
        const ImVec4 tint = ch.enabled
            ? ImVec4(1.f, 1.f, 1.f, 1.f)
            : ImVec4(0.3f, 0.3f, 0.3f, 0.7f);

        // Highlight the currently selected character (single or attack/defense)
        const bool isSingle = (m_selected == &ch);
        const bool isAttack = (m_selectedAttack == &ch);
        const bool isDefense = (m_selectedDefense == &ch);
        const bool isSelected = isSingle || isAttack || isDefense;
        if (isSelected) {
            ImVec4 highlightColor;
            if (isSingle)
                highlightColor = ImVec4(0.2f, 0.55f, 1.f, 1.f);   // blue (single pick)
            else if (isAttack && isDefense)
                highlightColor = ImVec4(0.6f, 0.1f, 0.8f, 1.f);   // purple (both roles)
            else if (isAttack)
                highlightColor = ImVec4(0.85f, 0.15f, 0.15f, 1.f); // red (attack)
            else
                highlightColor = ImVec4(0.1f, 0.35f, 0.85f, 1.f); // blue (defense)
            ImGui::PushStyleColor(ImGuiCol_Button, highlightColor);
        }

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
        }
        else {
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

        ConfigManager::Instance().Set(ch.name, ch.enabled);
        ConfigManager::Instance().Save();

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s  [%s]", ch.name.c_str(), ch.category.c_str());
            ImGui::Text("%s", ch.enabled ? "Enabled" : "Disabled");
            ImGui::EndTooltip();
        }

        ++col;
    }
}

void AppUI::RandomizeUpgradeMode()
{
    m_selectedUtilities = m_rng.PickUnique(kUtilityOptions, 2);
    m_selectedSecondaries = m_rng.PickUnique(kSecondaryOptions, 1);
    m_selectedMelee = m_rng.PickUnique(kMeleeOptions, 1);
    m_selectedAwakening = m_rng.PickUnique(kAwakeningOptions, 1);

    m_weaponUpgradeResults.clear();
    m_characterUpgradeResults.clear();
    m_abilityUpgradeResults.clear();

    auto pickCategory = [this](const UpgradeCategoryDefinition& def) {
        UpgradeCategoryResult result;
        result.name = def.name;
        result.upgrades = { "", "" }; // unused in category-name-only mode
        std::vector<int> indexes = { 0, 1 };
        const std::vector<int> picked = m_rng.PickUnique(indexes, 1);
        result.selectedIndex = picked.empty() ? 0 : picked.front();
        return result;
        };

    for (const auto& category : kWeaponUpgradeCategories)
        m_weaponUpgradeResults.push_back(pickCategory(category));
    for (const auto& category : kCharacterUpgradeCategories)
        m_characterUpgradeResults.push_back(pickCategory(category));
    for (const auto& category : kAbilityUpgradeCategories)
        m_abilityUpgradeResults.push_back(pickCategory(category));
}

void AppUI::RenderUpgradeRandomizerTab()
{
    // ── Prominent, centered randomize button ────────────────────────────────
    ImGui::Spacing();

    const float availWidth = ImGui::GetContentRegionAvail().x;
    const ImVec2 bigButtonSize(std::min(availWidth, 320.f), 44.f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - bigButtonSize.x) * 0.5f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 1.00f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.32f, 0.65f, 1.00f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.44f, 0.88f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushFont(gButtonFont);

    if (ImGui::Button("Randomize Loadout", bigButtonSize)) {
        RandomizeUpgradeMode();
    }

    ImGui::PopFont();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    auto renderPickList = [](const char* heading, const std::vector<std::string>& picks, std::size_t expectedCount) {
        ImGui::TextDisabled("%s", heading);

        if (picks.empty()) {
            ImGui::TextDisabled("  Press Randomize Loadout to generate picks.");
            return;
        }

        for (std::size_t i = 0; i < picks.size() && i < expectedCount; ++i)
            ImGui::BulletText("%s", picks[i].c_str());
        };

    // addTopSpacing controls whether a spacing is added before the group
    auto renderUpgradeGroup = [](const char* heading,
        const std::vector<UpgradeCategoryResult>& categories,
        const ImVec4& accentColor,
        bool addTopSpacing) {
            if (addTopSpacing)
                ImGui::Spacing();

            if (heading && heading[0] != '\0')
                ImGui::Text("%s", heading);

            for (const auto& category : categories) {
                ImGui::Text("%s", category.name.c_str());
                for (int i = 0; i < 2; ++i) {
                    const bool isSelected = (category.selectedIndex == i);
                    const ImVec4 selectedColor = accentColor;
                    const ImVec4 normalColor = ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.f);

                    ImGui::PushStyleColor(ImGuiCol_ChildBg, isSelected ? selectedColor : normalColor);
                    ImGui::PushStyleColor(ImGuiCol_Border, accentColor);

                    // Rounded corners + thicker border
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);

                    ImGui::PushID(category.name.c_str());
                    ImGui::PushID(i);

                    ImGui::BeginChild("upgrade_rect",ImVec2(260.f, 32.f),true,ImGuiWindowFlags_NoScrollbar |ImGuiWindowFlags_NoScrollWithMouse);
                    if (isSelected)
                    {
                        float textHeight = ImGui::GetTextLineHeight();
                        float y = (ImGui::GetWindowHeight() - textHeight) * 0.5f;

                        ImGui::SetCursorPosY(y);
                        ImGui::Text("%s", category.name.c_str());
                    }
                    ImGui::EndChild();

                    ImGui::PopID();
                    ImGui::PopID();

                    ImGui::PopStyleVar(2);
                    ImGui::PopStyleColor(2);
                }
                ImGui::Spacing();
            }
        };

    const ImVec4 weaponColor = ImVec4(0.20f, 0.55f, 1.00f, 1.f); // blue
    const ImVec4 characterColor = ImVec4(0.20f, 0.75f, 0.35f, 1.f); // green
    const ImVec4 abilityColor = ImVec4(0.95f, 0.85f, 0.20f, 1.f); // yellow

    renderPickList("UTILITY", m_selectedUtilities, 2);
    renderPickList("SECONDARY", m_selectedSecondaries, 1);
    renderPickList("MELEE", m_selectedMelee, 1);
    renderPickList("AWAKENING", m_selectedAwakening, 1);

    ImGui::Spacing();
    if (ImGui::BeginTable("UpgradeLayout", 2, ImGuiTableFlags_SizingStretchProp)) {
        // Left column
        ImGui::TableNextColumn();
        const float leftStartY = ImGui::GetCursorPosY();
        renderUpgradeGroup("", m_weaponUpgradeResults, weaponColor, false);
        const float leftEndY = ImGui::GetCursorPosY();
        const float leftHeight = leftEndY - leftStartY;

        // Right column (NO GAP between these two groups)
        ImGui::TableNextColumn();
        const float rightStartY = ImGui::GetCursorPosY();
        renderUpgradeGroup("", m_abilityUpgradeResults, abilityColor, false);
        renderUpgradeGroup("", m_characterUpgradeResults, characterColor, false);
        const float rightEndY = ImGui::GetCursorPosY();
        const float rightHeight = rightEndY - rightStartY;

        // Pad shorter side so bottoms align
        if (leftHeight < rightHeight) {
            ImGui::TableSetColumnIndex(0);
            ImGui::Dummy(ImVec2(0.f, rightHeight - leftHeight));
        }
        else if (rightHeight < leftHeight) {
            ImGui::TableSetColumnIndex(1);
            ImGui::Dummy(ImVec2(0.f, leftHeight - rightHeight));
        }

        ImGui::EndTable();
    }
}