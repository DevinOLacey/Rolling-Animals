#ifndef DICE_H
#define DICE_H

#include <string>
#include <exception>
#include <iostream>

class DiceException : public std::exception {
private:
    std::string msg;
public:
    DiceException(const std::string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

class Die {
protected:
    int sides;
public:
    Die(int s = 6);
    virtual ~Die() = default;

    virtual int roll() const;
    int getSides() const { return sides; }
    virtual std::string getType() const { return "Standard"; }

    virtual void printInfo() const {
        std::cout << getType() << " Die (" << sides << " sides)\n";
    }
};

#endif
