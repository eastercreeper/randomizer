#pragma once

#include "Character.h"
#include <random>
#include <vector>
#include <algorithm>
#include <cstddef>

// Picks one random character from an arbitrary pool of (const) pointers.
class Randomizer
{
public:
    Randomizer();

    // Returns a pointer into the pool, or nullptr when the pool is empty.
    const Character* Pick(const std::vector<const Character*>& pool, bool avoidImmediateRepeat = true);

    template <typename T>
    std::vector<T> PickUnique(const std::vector<T>& pool, std::size_t count)
    {
        if (pool.empty() || count == 0)
            return {};

        std::vector<T> picks = pool;
        std::shuffle(picks.begin(), picks.end(), m_rng);
        if (count < picks.size())
            picks.resize(count);
        return picks;
    }

private:
    void RefillBag(const std::vector<const Character*>& pool, bool avoidImmediateRepeat);

    std::mt19937 m_rng;

    std::vector<const Character*> m_bag;
    std::size_t m_index = 0;
    const Character* m_lastPick = nullptr;

};
