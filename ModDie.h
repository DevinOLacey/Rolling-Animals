#ifndef MOD_DIE_H
#define MOD_DIE_H

#include "MagicDice.h"


// Boost adds a flat amount to the player's roll
class BoostDice : public MagicDice {
private:
    int power;

public:
    BoostDice(int p = 2) : power(p) {}

    int applyEffect(int playerRoll, int) const override {
        return playerRoll + power;
    }

    std::string getType() const override { return "Boost"; }
    std::string getCategory() const override { return "Modifier"; }
};

// Shield reduces the opponent's roll by a flat amount (handled in main logic)
class ShieldDice : public MagicDice {
public:
    int applyEffect(int playerRoll, int) const override {
        return playerRoll; // handled in main logic
    }

    std::string getType() const override { return "Shield"; }
    std::string getCategory() const override { return "Modifier"; }
};


#endif
