#pragma once

#include "Character.h"
#include <vector>

// Picks one random character from an arbitrary pool of (const) pointers.
class Randomizer
{
public:
    // Returns a pointer into the pool, or nullptr when the pool is empty.
    const Character* Pick(const std::vector<const Character*>& pool);
};
