#include "Randomizer.h"

Randomizer::Randomizer()
    : m_rng(std::random_device{}())
{}

void Randomizer::RefillBag(const std::vector<const Character*>& pool, bool avoidImmediateRepeat) {
    m_bag = pool;

    if (m_bag.size() > 1) {
        std::shuffle(m_bag.begin(), m_bag.end(), m_rng);
        while (avoidImmediateRepeat && m_lastPick && m_bag.front() == m_lastPick) {
            std::shuffle(m_bag.begin(), m_bag.end(), m_rng);
        }

    }
    m_index = 0;
}

const Character* Randomizer::Pick(const std::vector<const Character*>& pool, bool avoidImmediateRepeat) {
    if (pool.empty()) return nullptr;

    const bool bagMatchesPool = (m_bag.size() == pool.size()) &&
        std::is_permutation(m_bag.begin(), m_bag.end(), pool.begin());

    if (m_bag.empty() || m_index >= m_bag.size() || !bagMatchesPool) {
        RefillBag(pool, avoidImmediateRepeat);
    }

    const Character* result = m_bag[m_index++];
    m_lastPick = result;

    return result;
}