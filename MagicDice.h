#ifndef MAGIC_DICE_H
#define MAGIC_DICE_H

#include "Dice.h"
#include <string>

class MagicDice : public Die {
public:
    MagicDice(int sides = 6) : Die(sides) {}
    virtual ~MagicDice() = default;

    // Effect logic
    virtual int applyEffect(int playerRoll, int opponentRoll) const = 0;

    // Check if the die is a mirror
    virtual bool isMirror() const { return false; }

    // Check if the die is a break
    virtual bool isBreak() const { return false; }

    // Name of the die
    virtual std::string getType() const = 0;

    // Category (used for activation rules)
    virtual std::string getCategory() const = 0;

    // Roll a d10 for activation
    int rollActivation() const;

    // Check if activation succeeds
    bool checkActivation(int roll) const;
};

#endif
