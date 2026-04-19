#ifndef TRANSFORM_DICE_H
#define TRANSFORM_DICE_H

#include "MagicDice.h"
#include <cstdlib>

// Base for dice that transform roll mathematically
class TransformDice : public MagicDice {
protected:
    int sides;

public:
    TransformDice(int s = 6) : sides(s) {}

    std::string getType() const override {
        return "Transform";
    }
};

// FLIP
class FlipDice : public TransformDice {
public:
    int applyEffect(int playerRoll, int) const override {
        return (sides + 1) - playerRoll;
    }

    std::string getType() const override {
        return "Flip";
    }
};

// REROLL
class RerollDice : public TransformDice {
public:
    int applyEffect(int, int) const override {
        return (rand() % sides) + 1;
    }

    std::string getType() const override {
        return "Reroll";
    }
};

// PILLAR
class PillarDice : public TransformDice {
public:
    int applyEffect(int playerRoll, int) const override {
        return playerRoll + (playerRoll - 1);
    }

    std::string getType() const override {
        return "Pillar";
    }
};

#endif