#include "Randomizer.h"

#include <random>

const Character* Randomizer::Pick(const std::vector<const Character*>& pool)
{
    if (pool.empty())
        return nullptr;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<std::size_t> dist(0, pool.size() - 1);
    return pool[dist(rng)];
}
