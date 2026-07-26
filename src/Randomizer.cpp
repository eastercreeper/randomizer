#include "Randomizer.h"

Randomizer::Randomizer()
    : m_rng(std::random_device{}())
{}

void Randomizer::RefillBag(const std::vector<const Character*>& pool) {
    m_bag = pool;

    if (m_bag.size() > 1) {
        do {
            std::shuffle(m_bag.begin(), m_bag.end(), m_rng);

        } 
        while (m_lastPick && m_bag.front() == m_lastPick);

    }
    m_index = 0;
}

const Character* Randomizer::Pick(const std::vector<const Character*>& pool) {
    if (pool.empty()) return nullptr;

    if (m_bag.empty() || m_index >= m_bag.size() != pool.size()) {
        RefillBag(pool);
    }

    const Character* result = m_bag[m_index++];
    m_lastPick = result;

    return result;
}