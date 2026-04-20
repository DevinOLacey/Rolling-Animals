#include "FileStorage.h"

#include "MagicDice.h"
#include "ModDie.h"
#include "OppDice.h"
#include "TransDie.h"
// No use making seprate headers for each magic die if they are all pretty simple and just override the same functions, might as well just put them like three headers for simplicity

#include <array>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {





    // MAGIC HELPER FUNCTION, creates a magic die based on the player's choice, returns nullptr if no magic is chosen or if the choice is invalid

    MagicDice* createMagicDice(int choice) {
        switch (choice) {
        case 1: return new BoostDice();
        case 2: return new ShieldDice();
        case 3: return new FlipDice();
        case 4: return new RerollDice();
        case 5: return new PillarDice();
        case 6: return new MirrorDice();
        case 7: return new BreakDice();
        default: return nullptr;
        }
    }

    MagicDice* createRandomMagicDice() {
        return createMagicDice((rand() % 7) + 1);
    }

    bool promptForYesNo(const std::string& message) {
        char choice;
        while (true) {
            std::cout << message;
            if (std::cin >> choice) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (choice == 'y' || choice == 'Y') {
                    return true;
                }
                if (choice == 'n' || choice == 'N') {
                    return false;
                }
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            std::cout << "Please enter y or n.\n";
        }
    }

    // EXISTING CODE, just moved down here for better organization

    // File paths zoobox is where the animal dice are stored, results is where the match history is stored, and animarecords is where the individual animal stats are stored (total wins, losses, etc.)

    const std::string kAnimalFile = "ZooBox.txt";
    const std::string kHistoryFile = "Results.txt";
    const std::string kStatsFile = "AnimalRecords.txt";

    void printDivider() {
        std::cout << "\n============================================================\n";
    }

    void printTitle() {
        printDivider();
        std::cout << "                  ANIMAL DIE BATTLE INTERFACE               \n";
        printDivider();
    }

    // Main menu display
    void printMainMenu() {
        std::cout << "\nMain Menu\n";
        std::cout << "1. Fight\n";
        std::cout << "2. Create a die\n";
        std::cout << "3. View animal dice\n";
        std::cout << "4. View previous matches / look up animal stats\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose an option: ";
    }

    void waitForEnter() {
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::system("cls");
    }

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

    // Formats the animal die's name and side values into a readable string
    std::string formatSideValues(const AnimalDieData& animal) {
        std::ostringstream output;
        output << animal.name << " d6";
        for (std::size_t i = 0; i < animal.sideValues.size(); ++i) {
            output << " s" << i + 1 << ":" << animal.sideValues[i];
        }
        return output.str();
    }

    // Rolls the animal die by randomly selecting one of its side values
    int rollAnimalDie(const AnimalDieData& animal) {
        int index = rand() % 6;
        return animal.sideValues[index];
    }

  

    // FIGHT LOGIC, runs a best of three series between two animal dice, with the new magic die mechanics integrated, and returns a record of the series for storage in the match history

    SeriesRecord runBestOfThreeSeries(const AnimalDieData& animalA, const AnimalDieData& animalB) {
        SeriesRecord record;
        record.animalA = animalA.name;
        record.animalB = animalB.name;
        record.animalAWins = 0;
        record.animalBWins = 0;

        // 🔥 NEW: magic cooldown tracking
        bool aUsedMagic = false;
        bool bUsedMagic = false;

        int gameNumber = 1;

        while (record.animalAWins < 2 && record.animalBWins < 2) {

            int animalARoll = rollAnimalDie(animalA);
            int animalBRoll = rollAnimalDie(animalB);

            std::cout << "\nGame " << gameNumber << " Base Rolls: "
                << animalA.name << " = " << animalARoll << ", "
                << animalB.name << " = " << animalBRoll << "\n";

            //  MAGIC SELECTION 

            bool useMagicA = false;
            if (!aUsedMagic) {
                useMagicA = promptForYesNo(animalA.name + " use magic this game? (y/n): ");
                if (useMagicA) {
                    aUsedMagic = true;
                }
            }
            else {
                std::cout << animalA.name << " has already used magic this series.\n";
            }

            bool useMagicB = false;
            if (!bUsedMagic) {
                useMagicB = promptForYesNo(animalB.name + " use magic this game? (y/n): ");
                if (useMagicB) {
                    bUsedMagic = true;
                }
            }
            else {
                std::cout << animalB.name << " has already used magic this series.\n";
            }

            MagicDice* magicA = useMagicA ? createRandomMagicDice() : nullptr;
            MagicDice* magicB = useMagicB ? createRandomMagicDice() : nullptr;

            bool aActive = false;
            bool bActive = false;

            //  ACTIVATION 

            if (magicA) {
                int roll = magicA->rollActivation();
                std::cout << animalA.name << " rolled " << roll << " for " << magicA->getType() << " magic\n";
                aActive = magicA->checkActivation(roll);
                if (!aActive) {
                    std::cout << animalA.name << "'s magic did not activate.\n";
                }
            }

            if (magicB) {
                int roll = magicB->rollActivation();
                std::cout << animalB.name << " rolled " << roll << " for " << magicB->getType() << " magic\n";
                bActive = magicB->checkActivation(roll);
                if (!bActive) {
                    std::cout << animalB.name << "'s magic did not activate.\n";
                }
            }

            //  BREAK 

            if (aActive && magicA && magicA->isBreak()) {
                std::cout << animalA.name << " used BREAK! Opponent magic canceled.\n";
                bActive = false;
            }

            if (bActive && magicB && magicB->isBreak()) {
                std::cout << animalB.name << " used BREAK! Opponent magic canceled.\n";
                aActive = false;
            }

            //  mirror/swap cause it was basically the same 

            if (aActive && magicA && magicA->isMirror()) {
                std::cout << animalA.name << " used Mirror!\n";
                std::swap(animalARoll, animalBRoll);
            }

            if (bActive && magicB && magicB->isMirror()) {
                std::cout << animalB.name << " used Mirror!\n";
                std::swap(animalARoll, animalBRoll);
            }

            //  OTHER EFFECTS 

            if (aActive && magicA && !magicA->isMirror() && !magicA->isBreak()) {
                animalARoll = magicA->applyEffect(animalARoll, animalBRoll);
            }

            if (bActive && magicB && !magicB->isMirror() && !magicB->isBreak()) {
                animalBRoll = magicB->applyEffect(animalBRoll, animalARoll);
            }

            // CLEANUP 

            delete magicA;
            delete magicB;

            // FINAL RESULT 

            std::cout << "Final Rolls: "
                << animalA.name << " = " << animalARoll << ", "
                << animalB.name << " = " << animalBRoll << "\n";

            if (animalARoll == animalBRoll) {
                std::cout << "Tie! Rerolling...\n";
                continue;
            }

            GameResult game;

            if (animalARoll > animalBRoll) {
                ++record.animalAWins;
                game = GameResult{ animalA.name, animalARoll, animalB.name, animalBRoll };
            }
            else {
                ++record.animalBWins;
                game = GameResult{ animalB.name, animalBRoll, animalA.name, animalARoll };
            }

            record.games.push_back(game);

            std::cout << animalA.name << " vs " << animalB.name
                << " -> " << game.winnerName
                << " wins with " << game.winnerRoll << "\n\n";

            ++gameNumber;
        }

        record.seriesWinner = (record.animalAWins > record.animalBWins)
            ? animalA.name
            : animalB.name;

        return record;
    }

    // Function to handle the "Fight" menu option, allowing the user to select two animal dice and run a best-of-three series.
    void fightAnimals() {
        // Load animal dice from file
        std::vector<AnimalDieData> animals = loadAnimalDice(kAnimalFile);
        if (animals.size() < 2) {
            std::cout << "Not enough animal dice to fight. Please create more.\n";
            return;
        }

        // Display available animal dice
        std::cout << "\nAvailable Animal Dice:\n";
        for (std::size_t i = 0; i < animals.size(); ++i) {
            std::cout << i + 1 << ". " << formatSideValues(animals[i]) << "\n";
        }

        // Select animal A
        int aIndex = promptForInteger("Select Animal A (1-" + std::to_string(animals.size()) + "): ", 1, static_cast<int>(animals.size())) - 1;
        // Select animal B
        int bIndex = promptForInteger("Select Animal B (1-" + std::to_string(animals.size()) + "): ", 1, static_cast<int>(animals.size())) - 1;
        if (aIndex == bIndex) {
            std::cout << "Cannot fight the same animal die against itself.\n";
            return;
        }

        // Run the best-of-three series
        SeriesRecord record = runBestOfThreeSeries(animals[aIndex], animals[bIndex]);

        // Display series result
        std::cout << "\nSeries Winner: " << record.seriesWinner << "\n";
        std::cout << "Games:\n";
        for (std::size_t i = 0; i < record.games.size(); ++i) {
            const GameResult& g = record.games[i];
            std::cout << "  Game " << i + 1 << ": " << g.winnerName << " (" << g.winnerRoll << ") defeated " << g.loserName << " (" << g.loserRoll << ")\n";
        }

        // Save match history
        appendSeriesRecord(kHistoryFile, record);
        // Update animal stats
        updateAnimalRecords(kStatsFile, record);
    }

    // Function to handle creating a new animal die and saving it to file
    void createDie() {
        std::vector<AnimalDieData> animals = loadAnimalDice(kAnimalFile);

        std::string name;
        std::cout << "\nEnter animal name: ";
        std::getline(std::cin, name);
        name = normalizeAnimalName(name);

        if (animalExists(animals, name)) {
            std::cout << "An animal die with that name already exists.\n";
            return;
        }

        std::array<int, 6> sides = { 0, 0, 0, 0, 0, 0 };
        for (int i = 0; i < 6; ++i) {
            sides[i] = promptForInteger("Enter value for side " + std::to_string(i + 1) + ": ", 1, 99);
        }

        AnimalDieData newAnimal{ name, sides };
        appendAnimalDie(kAnimalFile, newAnimal);
        std::cout << "Animal die \"" << name << "\" created and saved.\n";
    }

    // Displays all animal dice currently saved in the ZooBox file
    void viewAnimalDice() {
        std::vector<AnimalDieData> animals = loadAnimalDice(kAnimalFile);
        if (animals.empty()) {
            std::cout << "\nNo animal dice found. Please create some first.\n";
            return;
        }
        std::cout << "\nCurrent Animal Dice:\n";
        for (std::size_t i = 0; i < animals.size(); ++i) {
            std::cout << i + 1 << ". " << formatSideValues(animals[i]) << "\n";
        }
    }

    // Displays the match history menu and allows the user to view all matches or filter by animal name
    void viewMatchHistoryMenu() {
        const std::string kHistoryFile = "Results.txt";
        const std::string kStatsFile = "AnimalRecords.txt";

        std::vector<SeriesRecord> history = loadSeriesHistory(kHistoryFile);
        if (history.empty()) {
            std::cout << "\nNo match history found.\n";
            return;
        }

        std::cout << "\nMatch History Menu\n";
        std::cout << "1. View all matches\n";
        std::cout << "2. Look up animal stats\n";
        std::cout << "0. Return to main menu\n";
        std::cout << "Choose an option: ";

        int choice;
        while (!(std::cin >> choice) || choice < 0 || choice > 2) {
            std::cout << "Please enter a number from 0 to 2: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            std::cout << "\nAll Matches:\n";
            for (std::size_t i = 0; i < history.size(); ++i) {
                const SeriesRecord& rec = history[i];
                std::cout << "Match " << i + 1 << ": " << rec.animalA << " vs " << rec.animalB
                    << " | Winner: " << rec.seriesWinner
                    << " (" << rec.animalAWins << "-" << rec.animalBWins << ")\n";
            }
        } else if (choice == 2) {
            std::cout << "\nEnter animal name to look up: ";
            std::string name;
            std::getline(std::cin, name);
            name = normalizeAnimalName(name);

            std::vector<SeriesRecord> filtered = filterSeriesByAnimal(history, name);
            if (filtered.empty()) {
                std::cout << "No matches found for \"" << name << "\".\n";
            } else {
                std::cout << "\nMatches for " << name << ":\n";
                for (std::size_t i = 0; i < filtered.size(); ++i) {
                    const SeriesRecord& rec = filtered[i];
                    std::cout << "Match " << i + 1 << ": " << rec.animalA << " vs " << rec.animalB
                        << " | Winner: " << rec.seriesWinner
                        << " (" << rec.animalAWins << "-" << rec.animalBWins << ")\n";
                }
            }

            std::vector<AnimalRecord> stats = loadAnimalRecords(kStatsFile);
            const AnimalRecord* rec = findAnimalRecord(stats, name);
            if (rec) {
                std::cout << "\nStats for " << rec->name << ":\n";
                std::cout << "  Series Wins: " << rec->seriesWins << "\n";
                std::cout << "  Series Losses: " << rec->seriesLosses << "\n";
                std::cout << "  Game Wins: " << rec->gameWins << "\n";
                std::cout << "  Game Losses: " << rec->gameLosses << "\n";
                std::cout << "  Average Roll: ";
                if (rec->rollCount > 0)
                    std::cout << std::fixed << std::setprecision(2) << computeAverageRoll(*rec) << "\n";
                else
                    std::cout << "N/A\n";
            } else {
                std::cout << "No stats found for \"" << name << "\".\n";
            }
        }
        // If choice == 0, just return to main menu
    }

} // closing brace for name space

//  MAIN, where the magic happens, aka the main menu loop and function calls, also initializes random seed for die rolls

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    bool running = true;
    printTitle();

    while (running) {
        printMainMenu();
        int choice = promptForInteger("", 0, 4);

        if (choice == 1) {
            fightAnimals();
            waitForEnter();
        }
        else if (choice == 2) {
            createDie();
            waitForEnter();
        }
        else if (choice == 3) {
            viewAnimalDice();
            waitForEnter();
        }
        else if (choice == 4) {
            viewMatchHistoryMenu();
            waitForEnter();
        }
        else if (choice == 0) {
            running = false;
        }
    }

    // Cleanup and exit
    printDivider();
    std::cout << "Closing prototype.\n";
    return 0;
}
