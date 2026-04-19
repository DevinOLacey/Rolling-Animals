#include "Dice.h"
#include <cstdlib> // for rand()

Die::Die(int s) {
    if (s < 2) {
        throw DiceException("Die must have at least 2 sides.");
    }
    sides = s;
}

int Die::roll() const {
    return (rand() % sides) + 1;
}
