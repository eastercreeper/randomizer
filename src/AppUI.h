#pragma once

#include "CharacterManager.h"
#include "Randomizer.h"

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

    CharacterManager& m_mgr;
    Randomizer&       m_rng;

    bool m_filterPus      = true;
    bool m_filterScissors = true;
    bool m_filterUrbino   = true;

    const Character* m_selected = nullptr;

    static constexpr float kThumbSize  = 80.0f;
    static constexpr float kResultSize = 256.0f;
};
