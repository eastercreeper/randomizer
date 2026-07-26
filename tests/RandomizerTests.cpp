#include "Randomizer.h"

#include <iostream>
#include <vector>

int main() {
    Character urbino{"urbino", "urbino", "", 0, 0, 0, true};
    Character scissors{"scissors", "scissors", "", 0, 0, 0, true};
    Character pusA{"pusA", "pus", "", 0, 0, 0, true};
    Character pusB{"pusB", "pus", "", 0, 0, 0, true};

    {
        Randomizer rng;
        std::vector<const Character*> onlyOne{&urbino};
        for (int i = 0; i < 1000; ++i) {
            if (rng.Pick(onlyOne) != &urbino) {
                std::cerr << "single-selection pick mismatch\n";
                return 1;
            }
        }
    }

    {
        Randomizer rng;
        std::vector<const Character*> defensePool{&urbino};
        std::vector<const Character*> attackPool{&urbino, &scissors};
        bool sawSameCharacter = false;
        for (int i = 0; i < 200; ++i) {
            const Character* defense = rng.Pick(defensePool);
            const Character* attack = rng.Pick(attackPool, false);
            if (defense == attack) {
                sawSameCharacter = true;
                break;
            }
        }
        if (!sawSameCharacter) {
            std::cerr << "never selected shared Urbino character for both sides\n";
            return 1;
        }
    }

    {
        Randomizer rng;
        std::vector<const Character*> twoChoices{&pusA, &pusB};
        const Character* previous = nullptr;
        for (int i = 0; i < 200; ++i) {
            const Character* current = rng.Pick(twoChoices);
            if (previous && current == previous) {
                std::cerr << "unexpected immediate duplicate with default anti-repeat\n";
                return 1;
            }
            previous = current;
        }
    }

    return 0;
}
