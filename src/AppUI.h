#pragma once

#include "CharacterManager.h"
#include "Randomizer.h"
#include <array>
#include <string>
#include <vector>

// Owns all ImGui rendering for one frame.
class AppUI
{
public:
    AppUI(CharacterManager& mgr, Randomizer& rng);

    // Call once per frame between ImGui::NewFrame() and ImGui::Render().
    void Render();

private:
    void RenderFilterBar();
    void RenderCharacterGrid();
    void RenderResultPanel();
    void RenderUpgradeRandomizerTab();
    void RandomizeUpgradeMode();

    CharacterManager& m_mgr;
    Randomizer&       m_rng;

    bool m_filterPus      = true;
    bool m_filterScissors = true;
    bool m_filterUrbino   = true;

    const Character* m_selected        = nullptr;
    const Character* m_selectedAttack  = nullptr;
    const Character* m_selectedDefense = nullptr;

    struct UpgradeCategoryResult {
        std::string name;
        std::array<std::string, 2> upgrades;
        int selectedIndex = -1;
    };

    std::vector<std::string>        m_selectedUtilities;
    std::vector<std::string>        m_selectedSecondaries;
    std::vector<std::string>        m_selectedMelee;
    std::vector<std::string>        m_selectedAwakening;
    std::vector<UpgradeCategoryResult> m_weaponUpgradeResults;
    std::vector<UpgradeCategoryResult> m_characterUpgradeResults;
    std::vector<UpgradeCategoryResult> m_abilityUpgradeResults;

    static constexpr float kThumbSize  = 120.0f;
    static constexpr float kResultSize = 400.0f;
};
