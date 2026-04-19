#ifndef DICE_H
#define DICE_H

#include <string>
#include <exception>
#include <iostream>
using namespace std;

class DiceException : public exception {
private:
    string msg;
public:
    DiceException(const string& m) : msg(m) {}
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
    virtual string getType() const { return "Standard"; }

    virtual void printInfo() const {
        cout << getType() << " Die (" << sides << " sides)\n";
    }
};

#endif
