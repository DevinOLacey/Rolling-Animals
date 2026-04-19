#ifndef MODIFIER_DICE_H
#define MODIFIER_DICE_H

#include "MagicDice.h"

// Base for dice that modify player roll
class ModifierDice : public MagicDice {
protected:
    int power;

public:
    ModifierDice(int p = 1) : power(p) {}

    std::string getType() const override {
        return "Modifier";
    }
};

// BOOST
class BoostDice : public ModifierDice {
public:
    BoostDice(int p = 2) : ModifierDice(p) {}

    int applyEffect(int playerRoll, int) const override {
        return playerRoll + power;
    }

    std::string getType() const override {
        return "Boost";
    }
};

// SHIELD
class ShieldDice : public ModifierDice {
public:
    ShieldDice(int p = 2) : ModifierDice(p) {}

    int applyEffect(int playerRoll, int) const override {
        return playerRoll;
    }

    std::string getType() const override {
        return "Shield";
    }
};

// BURN
class BurnDice : public ModifierDice {
public:
    BurnDice(int p = 3) : ModifierDice(p) {}

    int applyEffect(int playerRoll, int) const override {
        return playerRoll + power;
    }

    std::string getType() const override {
        return "Burn";
    }
};

#endif