#pragma once

#include "Character.h"
#include <random>
#include <vector>
#include <algorithm>

// Picks one random character from an arbitrary pool of (const) pointers.
class Randomizer
{
public:
    Randomizer();

    // Returns a pointer into the pool, or nullptr when the pool is empty.
    const Character* Pick(const std::vector<const Character*>& pool, bool avoidImmediateRepeat = true);

private:
    void RefillBag(const std::vector<const Character*>& pool, bool avoidImmediateRepeat);

    std::mt19937 m_rng;

    std::vector<const Character*> m_bag;
    std::size_t m_index = 0;
    const Character* m_lastPick = nullptr;

};
