#include "Randomizer.h"

Randomizer::Randomizer()
    : m_rng(std::random_device{}())
{}

const Character* Randomizer::Pick(const std::vector<const Character*>& pool)
{
    if (pool.empty())
        return nullptr;

    std::uniform_int_distribution<std::size_t> dist(0, pool.size() - 1);
    return pool[dist(m_rng)];
}
