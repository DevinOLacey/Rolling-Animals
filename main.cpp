#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

/*
    Animal Die Battle Interface
    ------------------------------------------------------------
    This file contains the current single-file C++ prototype for
    the project interface.

    What this file currently does:
    - Shows the main menu.
    - Lets the user create an animal die and will display it iin the view option but does not save it to a file.
    - Lets the user view animal dice already in memory.

    Dice rule for this prototype:
    - Every animal uses a d6. *unsure if this should be changed*
    - Each animal stores the value printed on all 6 sides.

    Example:
    Dog d6 s1:3 s2:3 s3:3 s4:4 s5:5 s6:6

    TO DO:
    1. create a dice class
    2. create a file to store comabt data
    3. write to that file the records matches w/l and the animanls in the abttle as well as what each ones rolled
    4. create another class to inherit from dice (if we want users to make their owwn dice i think we should make a class that builds itself from reading a file)
    5. move logic fromt his file to its own as needed
*/

// Stores one animal's name and the six face values on its d6.
struct AnimalDie {
    std::string name;
    std::vector<int> sideValues;
};

// Prints a divider to separate interface screens.
void printDivider() {
    std::cout << "\n============================================================\n";
}

// Prints the title shown when the program starts.
void printTitle() {
    printDivider();
    std::cout << "                  ANIMAL DIE BATTLE INTERFACE               \n";
    printDivider();
}

// Prints the main navigation menu.
void printMainMenu() {
    std::cout << "\nMain Menu\n";
    std::cout << "1. Fight\n";
    std::cout << "2. Create a die\n";
    std::cout << "3. View animal dice\n";
    std::cout << "4. View previous matches / look up animal stats\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";
}

// Waits for the user to press Enter before returning to the menu.
void waitForEnter() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Repeats until the user enters a valid integer inside the allowed range.
int promptForInteger(const std::string& message, int minValue, int maxValue) {
    int value;

    while (true) {
        std::cout << message;

        if (std::cin >> value && value >= minValue && value <= maxValue) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Please enter a number from " << minValue << " to " << maxValue << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Repeats until the user enters non-empty text.
std::string promptForText(const std::string& message) {
    std::string value;

    while (true) {
        std::cout << message;
        std::getline(std::cin, value);

        if (!value.empty()) {
            return value;
        }

        std::cout << "Input cannot be empty.\n";
    }
}

// Shows a placeholder screen for any route that is not implemented yet.
void showComingSoon(const std::string& routeName) {
    printDivider();
    std::cout << routeName << "\n";
    std::cout << "Coming soon.\n";
}

// Builds the display text for one animal die and all six side values. to be moved most likely to its own file
std::string formatSideValues(const AnimalDie& animal) {
    std::ostringstream output;
    output << animal.name << " d6";

    for (std::size_t index = 0; index < animal.sideValues.size(); ++index) {
        output << " s" << index + 1 << ":" << animal.sideValues[index];
    }

    return output.str();
}

// Shows the list of animal dice currently loaded in the interface. should come from the inherited class eventually
void viewAnimalDice(const std::vector<AnimalDie>& animals) {
    printDivider();
    std::cout << "Animal Dice\n";

    if (animals.empty()) {
        std::cout << "No animal dice have been created yet.\n";
        return;
    }

    for (std::size_t index = 0; index < animals.size(); ++index) {
        const AnimalDie& animal = animals[index];
        std::cout << index + 1 << ". " << formatSideValues(animal) << "\n";
    }
}

// Collects user input for a new d6 animal die and adds it to the list. should be a class funtion
void createDie(std::vector<AnimalDie>& animals) {
    printDivider();
    std::cout << "Create A Die\n";

    AnimalDie newAnimal;
    newAnimal.name = promptForText("Enter the animal name: ");
    std::cout << "All animal dice are d6.\n";

    for (int sideNumber = 1; sideNumber <= 6; ++sideNumber) {
        int sideValue = promptForInteger("Enter value for side " + std::to_string(sideNumber) + ": ", 1, 99);
        newAnimal.sideValues.push_back(sideValue);
    }

    animals.push_back(newAnimal);

    std::cout << "\nInterface preview only:\n";
    std::cout << formatSideValues(newAnimal) << "\n";
    std::cout << "Saving and validation logic: Coming soon.\n";
}

int main() {
    // Starter example so the interface has sample data on launch.
    std::vector<AnimalDie> animals = {
        {"Dog", {3, 3, 3, 4, 5, 6}}
    };

    bool running = true;
    printTitle();

    while (running) {
        printMainMenu();
        int choice = promptForInteger("", 0, 4);

        if (choice == 1) {
            showComingSoon("Fight");
            waitForEnter();
        } else if (choice == 2) {
            createDie(animals);
            waitForEnter();
        } else if (choice == 3) {
            viewAnimalDice(animals);
            waitForEnter();
        } else if (choice == 4) {
            showComingSoon("View Previous Matches / Look Up Animal Stats");
            waitForEnter();
        } else if (choice == 0) {
            running = false;
        }
    }

    printDivider();
    std::cout << "Closing prototype.\n";
    return 0;
}
