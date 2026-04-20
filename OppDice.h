#ifndef OPP_DICE_H
#define OPP_DICE_H

#include "MagicDice.h"

// Mirror swaps the player's rolls, so the player's roll becomes the opponent's and vice versa.
class MirrorDice : public MagicDice {
public:
    int applyEffect(int playerRoll, int opponentRoll) const override {
        return playerRoll; // handled in main
    }

    std::string getType() const override { return "Swap"; }
    std::string getCategory() const override { return "Opponent"; }

    bool isMirror() const override { return true; }
};

// Break cancels the opponent's magic die effect, so the opponent's roll is unaffected by their magic die.
class BreakDice : public MagicDice {
public:
    int applyEffect(int playerRoll, int opponentRoll) const override {
        return playerRoll; // handled in main
    }

    std::string getType() const override { return "Break"; }
    std::string getCategory() const override { return "Opponent"; }

    bool isBreak() const override { return true; }
};

#endif
