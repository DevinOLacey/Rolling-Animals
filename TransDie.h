#ifndef TRANSFORM_DICE_H
#define TRANSFORM_DICE_H

#include "MagicDice.h"
#include <cstdlib>

// FLIP, filps the player's roll to the opposite side of the die (1 becomes 6, 2 becomes 5, etc.)
class FlipDice : public MagicDice {
public:
    int applyEffect(int playerRoll, int) const override {
        return 7 - playerRoll;
    }

    std::string getType() const override { return "Flip"; }
    std::string getCategory() const override { return "Transform"; }
};

// REROLL, rerolls the player's die and takes the new result instead of the original roll.
class RerollDice : public MagicDice {
public:
    int applyEffect(int, int) const override {
        return (rand() % 6) + 1;
    }

    std::string getType() const override { return "Reroll"; }
    std::string getCategory() const override { return "Transform"; }
};

// Pillar, adds the bottom of the "die" to the player's roll, EX: if the player rolls a 2 , they get a boost of 5 (7 - 2) for a total of 7. If they roll a 5, they get a boost of 2 (7 - 5) for a total of 7. 
// This is the most powerful transform die, since it can guarantee a decently high roll, but it is also the hardest to activate since it can be very powerful.
class PillarDice : public MagicDice {
public:
    int applyEffect(int playerRoll, int) const override {
        return playerRoll + (7 - playerRoll);
    }

    std::string getType() const override { return "Pillar"; }
    std::string getCategory() const override { return "Transform"; }
};

#endif
