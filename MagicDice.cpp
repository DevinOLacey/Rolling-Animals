#include "MagicDice.h"
#include <cstdlib>

int MagicDice::rollActivation() const {
    return (rand() % 10) + 1;
}

bool MagicDice::checkActivation(int roll) const {
    std::string type = getCategory();
	std::string name = getType();

	if (type == "Modifier") return roll >= 4;     // a lot easier compared to the others, since modifiers are generally less powerful
	if (type == "Transform" && name == "Pillar") return roll >= 7;    // Pillar is the most powerful transform die, so it is the hardest to activate
	else if (type == "Transform") return roll >= 5;     // easier than opponent magic, since transforms only affect the player's own roll usually, so they are less likely to completely change the outcome of the game
    if (type == "Opponent") return roll >= 6;     // harder

    return false;
}