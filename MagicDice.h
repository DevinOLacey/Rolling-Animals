#ifndef MAGIC_DICE_H
#define MAGIC_DICE_H

#include "Dice.h"
#include <string>

// Base class for all magic dice types
class MagicDice : public Die {
public:
    MagicDice(int sides = 6) : Die(sides) {}
    virtual ~MagicDice() = default;

    // Every magic die MUST define its own effect
    virtual int applyEffect(int playerRoll, int opponentRoll) const = 0;

    virtual std::string getType() const {
        return "Magic";
    }
};

#endif