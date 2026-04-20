#ifndef ANIMAL_DIE_H
#define ANIMAL_DIE_H

#include "Dice.h"
#include <string>

class AnimalDie : public Die {

protected:
    std::string animalType;   // Type of animal (Lion, Tiger, etc.)
    int animalValues[6];      // Values for each side of the die

public:
    AnimalDie();
    AnimalDie(const std::string& type, const int values[6]);

    int roll() const override;

    std::string getAnimalType() const;

    void printInfo() const override;

    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
};

#endif
