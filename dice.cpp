#include "Dice.h"
#include <cstdlib> // for rand()

Die::Die(int s) {
    if (s < 1) {
        throw DiceException("Die must have at least 1 side.");
    }
    sides = s;
}

int Die::roll() const {
    return (rand() % sides) + 1;
}
