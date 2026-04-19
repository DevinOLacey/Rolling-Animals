#ifndef OPPONENT_DICE_H
#define OPPONENT_DICE_H

#include "MagicDice.h"

// Base for opponent-interaction dice
class OpponentDice : public MagicDice {
public:
    std::string getType() const override {
        return "Opponent";
    }
};

// MIRROR
class MirrorDice : public OpponentDice {
public:
    int applyEffect(int, int opponentRoll) const override {
        return opponentRoll;
    }

    std::string getType() const override {
        return "Mirror";
    }
};

// SWAP
class SwapDice : public OpponentDice {
public:
    int applyEffect(int, int opponentRoll) const override {
        return opponentRoll;
    }

    std::string getType() const override {
        return "Swap";
    }
};

// BREAK
class BreakDice : public OpponentDice {
public:
    int applyEffect(int playerRoll, int) const override {
        return playerRoll;
    }

    std::string getType() const override {
        return "Break";
    }
};

#endif