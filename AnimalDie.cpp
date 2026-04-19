#include "AnimalDie.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

// Default constructor (standard 1–6 die)
AnimalDie::AnimalDie() : Die(6), animalType("Default") {
    for (int i = 0; i < 6; i++) {
        animalValues[i] = i + 1;
    }
}

// Custom constructor
AnimalDie::AnimalDie(const std::string& type, const int values[6])
    : Die(6), animalType(type) {
    for (int i = 0; i < 6; i++) {
        animalValues[i] = values[i];
    }
}

// Roll the die (random side)
int AnimalDie::roll() const {
    int index = rand() % 6;
    return animalValues[index];
}

// Get animal type
std::string AnimalDie::getAnimalType() const {
    return animalType;
}

// Print die info
void AnimalDie::printInfo() const {
    std::cout << animalType << " Die: ";
    for (int i = 0; i < 6; i++) {
        std::cout << animalValues[i] << " ";
    }
    std::cout << "\n";
}

// Load die from file
// Format: AnimalName v1 v2 v3 v4 v5 v6
bool AnimalDie::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file) return false;

    file >> animalType;

    for (int i = 0; i < 6; i++) {
        file >> animalValues[i];
    }

    return true;
}

// Save die to file
bool AnimalDie::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);

    if (!file) return false;

    file << animalType << " ";

    for (int i = 0; i < 6; i++) {
        file << animalValues[i] << " ";
    }

    return true;
}
