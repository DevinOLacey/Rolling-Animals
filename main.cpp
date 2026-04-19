#include "FileStorage.h"

#include <array>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {

// These are the text files the program uses to save data.
// ZooBox.txt stores the animal dice.
// Results.txt stores the history of finished fights.
// AnimalRecords.txt stores the running win/loss stats for each animal.
const std::string kAnimalFile = "ZooBox.txt";
const std::string kHistoryFile = "Results.txt";
const std::string kStatsFile = "AnimalRecords.txt";

// Prints a line across the screen to make the menu easier to read.
void printDivider() {
    std::cout << "\n============================================================\n";
}

// Prints the program title at the top of the screen.
void printTitle() {
    printDivider();
    std::cout << "                  ANIMAL DIE BATTLE INTERFACE               \n";
    printDivider();
}

// Shows the main menu options the user can choose from.
void printMainMenu() {
    std::cout << "\nMain Menu\n";
    std::cout << "1. Fight\n";
    std::cout << "2. Create a die\n";
    std::cout << "3. View animal dice\n";
    std::cout << "4. View previous matches / look up animal stats\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";
}

// Waits for the user to press Enter before clearing the screen.
void waitForEnter() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    // "cls" clears the terminal.
    std::system("cls");
}

// Keeps asking the user for a number until they enter a valid one that is inside the allowed range.
int promptForInteger(const std::string& message, int minValue, int maxValue) {
    int value;

    while (true) {
        std::cout << message;

        if (std::cin >> value && value >= minValue && value <= maxValue) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Please enter a number from " << minValue << " to " << maxValue << ".\n";
        // Clear the error state so cin can be used again.
        std::cin.clear();
        // Throw away the rest of the bad input line.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Keeps asking for text until the user enters something that is not blank.
std::string promptForText(const std::string& message) {
    std::string value;

    while (true) {
        std::cout << message;
        std::getline(std::cin, value);

        // normalizeAnimalName removes extra spaces, so a line of only spaces should still count as empty input.
        if (!normalizeAnimalName(value).empty()) {
            return value;
        }

        std::cout << "Input cannot be empty.\n";
    }
}

// Builds one readable line that shows an animal's name and all 6 values on its die.
std::string formatSideValues(const AnimalDieData& animal) {
    std::ostringstream output;
    output << animal.name << " d6";

    for (std::size_t index = 0; index < animal.sideValues.size(); ++index) {
        output << " s" << index + 1 << ":" << animal.sideValues[index];
    }

    return output.str();
}

// Picks one of the 6 sides at random and returns that value.
int rollAnimalDie(const AnimalDieData& animal) {
    int sideIndex = rand() % static_cast<int>(animal.sideValues.size());
    return animal.sideValues[static_cast<std::size_t>(sideIndex)];
}

// Loads every saved animal die from the file and prints them to the screen.
void viewAnimalDice() {
    printDivider();
    std::cout << "Animal Dice\n";

    const std::vector<AnimalDieData> animals = loadAnimalDice(kAnimalFile);
    if (animals.empty()) {
        std::cout << "No animal dice have been created yet.\n";
        return;
    }

    for (std::size_t index = 0; index < animals.size(); ++index) {
        std::cout << index + 1 << ". " << formatSideValues(animals[index]) << "\n";
    }
}

// Lets the user create a new animal die, checks that the name is unique,
// and saves it to the animal file.
void createDie() {
    printDivider();
    std::cout << "Create A Die\n";

    std::vector<AnimalDieData> animals = loadAnimalDice(kAnimalFile);
    AnimalDieData newAnimal;
    std::string rawName;

    while (true) {
        rawName = promptForText("Enter the animal name: ");
        newAnimal.name = normalizeAnimalName(rawName);

        // Do not allow two saved animals to have the same name.
        if (animalExists(animals, newAnimal.name)) {
            std::cout << "That animal name already exists. Choose a unique name.\n";
        } else {
            break;
        }
    }

    std::cout << "All animal dice are d6.\n";
    // Ask for the value on each of the 6 sides of the die.
    for (std::size_t side = 0; side < newAnimal.sideValues.size(); ++side) {
        newAnimal.sideValues[side] = promptForInteger(
            "Enter value for side " + std::to_string(side + 1) + ": ",
            1,
            99
        );
    }

    if (!appendAnimalDie(kAnimalFile, newAnimal)) {
        std::cout << "Could not save the die to " << kAnimalFile << ".\n";
        return;
    }

    std::cout << "\nSaved:\n";
    std::cout << formatSideValues(newAnimal) << "\n";
}

// Shows all saved animals, asks the user to pick one,
// and returns its position in the vector.
int promptForAnimalSelection(const std::vector<AnimalDieData>& animals, const std::string& message) {
    for (std::size_t index = 0; index < animals.size(); ++index) {
        std::cout << index + 1 << ". " << formatSideValues(animals[index]) << "\n";
    }

    return promptForInteger(message, 1, static_cast<int>(animals.size())) - 1;
}

// Runs a best-of-3 fight between two animals.
// The first animal to win 2 games wins the series.
// Tied rolls do not count and are rolled again.
SeriesRecord runBestOfThreeSeries(const AnimalDieData& animalA, const AnimalDieData& animalB) {
    SeriesRecord record;
    record.animalA = animalA.name;
    record.animalB = animalB.name;
    record.animalAWins = 0;
    record.animalBWins = 0;

    int gameNumber = 1;
    while (record.animalAWins < 2 && record.animalBWins < 2) {
        int animalARoll = rollAnimalDie(animalA);
        int animalBRoll = rollAnimalDie(animalB);

        if (animalARoll == animalBRoll) {
            // A tie does not count as a game win for either side.
            std::cout << "Game " << gameNumber << ": " << animalA.name << " and " << animalB.name
                      << " tied with " << animalARoll << ". Rerolling that game.\n\n";
            continue;
        }

        GameResult game;
        if (animalARoll > animalBRoll) {
            ++record.animalAWins;
            game = GameResult{animalA.name, animalARoll, animalB.name, animalBRoll};
        } else {
            ++record.animalBWins;
            game = GameResult{animalB.name, animalBRoll, animalA.name, animalARoll};
        }

        // Save the result of this counted game so it can be written to Results.txt later.
        record.games.push_back(game);

        std::cout << "Game " << gameNumber << ": " << animalA.name << " vs " << animalB.name
                  << " : " << game.winnerName << " wins with a " << game.winnerRoll
                  << " beating " << game.loserName << "'s " << game.loserRoll << "\n\n";

        ++gameNumber;
    }

    // After the loop ends, one animal must have won the series.
    record.seriesWinner = (record.animalAWins > record.animalBWins) ? animalA.name : animalB.name;
    return record;
}

// Prints the final result of the full best-of-3 series.
void printSeriesSummary(const SeriesRecord& record) {
    std::cout << "\nSeries Summary\n";
    std::cout << record.animalA << " vs " << record.animalB << " -> "
              << record.seriesWinner << " wins the series "
              << record.animalAWins << "-" << record.animalBWins << "\n";
}

// Lets the user choose two saved animals, runs the fight,
// saves the match history, and updates the running animal stats.
void fightAnimals() {
    printDivider();
    std::cout << "Fight\n";

    const std::vector<AnimalDieData> animals = loadAnimalDice(kAnimalFile);
    // A fight is only possible if at least two animals have been created.
    if (animals.size() < 2) {
        std::cout << "You need at least two saved animal dice before fighting.\n";
        return;
    }

    std::cout << "Choose the first animal:\n";
    const int firstIndex = promptForAnimalSelection(animals, "Choose a number: ");

    int secondIndex = -1;
    // Do not allow the same animal to fight itself.
    while (true) {
        std::cout << "\nChoose the second animal:\n";
        secondIndex = promptForAnimalSelection(animals, "Choose a number: ");

        if (secondIndex == firstIndex) {
            std::cout << "Choose a different animal for the second fighter.\n";
        } else {
            break;
        }
    }

    std::cout << "\n";
    SeriesRecord record = runBestOfThreeSeries(animals[static_cast<std::size_t>(firstIndex)], animals[static_cast<std::size_t>(secondIndex)]);
    printSeriesSummary(record);

    // Save this finished series to the match history file.
    if (!appendSeriesRecord(kHistoryFile, record)) {
        std::cout << "Could not save the series history.\n";
        return;
    }

    // Update the overall win/loss records and average roll data.
    if (!updateAnimalRecords(kStatsFile, record)) {
        std::cout << "Could not update animal records.\n";
        return;
    }

    std::cout << "Series history and animal records saved.\n";

    // TODO: Add magic die effect when they are implemented.
}

// Prints one or more saved series records in a readable format.
void printHistoryRecords(const std::vector<SeriesRecord>& records) {
    if (records.empty()) {
        std::cout << "No match history found.\n";
        return;
    }

    for (std::size_t recordIndex = 0; recordIndex < records.size(); ++recordIndex) {
        const SeriesRecord& record = records[recordIndex];
        std::cout << "\nSeries " << recordIndex + 1 << ": " << record.animalA << " vs "
                  << record.animalB << " -> " << record.seriesWinner << " won "
                  << record.animalAWins << "-" << record.animalBWins << "\n";

        for (std::size_t gameIndex = 0; gameIndex < record.games.size(); ++gameIndex) {
            const GameResult& game = record.games[gameIndex];
            std::cout << "  Game " << gameIndex + 1 << ": " << game.winnerName
                      << " won with " << game.winnerRoll << " over "
                      << game.loserName << "'s " << game.loserRoll << "\n";
        }
    }
}

// Looks up one animal by name and shows both its die values
// and its saved win/loss stats.
void viewAnimalRecord() {
    printDivider();
    std::cout << "Animal Record Lookup\n";

    const std::vector<AnimalDieData> animals = loadAnimalDice(kAnimalFile);
    const std::vector<AnimalRecord> records = loadAnimalRecords(kStatsFile);
    if (animals.empty()) {
        std::cout << "No saved animal dice were found.\n";
        return;
    }

    const std::string requestedName = normalizeAnimalName(promptForText("Enter animal name: "));
    const AnimalRecord* record = findAnimalRecord(records, requestedName);

    const AnimalDieData* animal = nullptr;
    // Find the actual die so its 6 side values can be displayed.
    for (const AnimalDieData& current : animals) {
        if (namesMatch(current.name, requestedName)) {
            animal = &current;
            break;
        }
    }

    if (animal == nullptr) {
        std::cout << "That animal was not found.\n";
        return;
    }

    std::cout << formatSideValues(*animal) << "\n";

    if (record == nullptr) {
        std::cout << "Series Record: 0-0\n";
        std::cout << "Series Win Rate: No completed series yet\n";
        std::cout << "Game Record: 0-0\n";
        std::cout << "Game Win Rate: No counted games yet\n";
        std::cout << "Average Roll: No counted games yet\n";
    } else {
        // Convert win/loss totals into percentages for display.
        const int totalSeries = record->seriesWins + record->seriesLosses;
        const int totalGames = record->gameWins + record->gameLosses;
        const double seriesWinRate = (totalSeries == 0)
            ? 0.0
            : (static_cast<double>(record->seriesWins) / static_cast<double>(totalSeries)) * 100.0;
        const double gameWinRate = (totalGames == 0)
            ? 0.0
            : (static_cast<double>(record->gameWins) / static_cast<double>(totalGames)) * 100.0;

        std::cout << "Series Record: " << record->seriesWins << "-" << record->seriesLosses << "\n";
        std::cout << std::fixed << std::setprecision(2)
                  << "Series Win Rate: " << seriesWinRate << "%\n";
        std::cout << "Game Record: " << record->gameWins << "-" << record->gameLosses << "\n";
        std::cout << "Game Win Rate: " << gameWinRate << "%\n";
        // Average Roll is based on real battle rolls that were recorded,
        // not the average of the die's 6 side values.
        std::cout << std::fixed << std::setprecision(2)
                  << "Average Roll: " << computeAverageRoll(*record) << "\n";
    }
}

// Shows the submenu for match history and animal stat lookup.
void viewMatchHistoryMenu() {
    printDivider();
    std::cout << "History And Stats\n";
    std::cout << "1. View all match history\n";
    std::cout << "2. Filter match history by animal\n";
    std::cout << "3. Look up animal stats\n";
    std::cout << "0. Back\n";

    const int choice = promptForInteger("Choose an option: ", 0, 3);
    if (choice == 0) {
        return;
    }

    if (choice == 1) {
        printDivider();
        std::cout << "All Match History\n";
        printHistoryRecords(loadSeriesHistory(kHistoryFile));
    } else if (choice == 2) {
        printDivider();
        std::cout << "Filtered Match History\n";
        const std::string animalName = normalizeAnimalName(promptForText("Enter animal name: "));
        const std::vector<SeriesRecord> allRecords = loadSeriesHistory(kHistoryFile);
        printHistoryRecords(filterSeriesByAnimal(allRecords, animalName));
    } else {
        viewAnimalRecord();
    }
}

}

// Starts the program, seeds the random number generator,
// shows the menu, and keeps running until the user chooses Exit.
int main() {
    // Seed rand() once so the rolls are different each time the program runs.
    srand(static_cast<unsigned int>(time(nullptr)));

    bool running = true;
    printTitle();

    // Keep showing the menu until the user chooses to quit.
    while (running) {
        printMainMenu();
        int choice = promptForInteger("", 0, 4);

        if (choice == 1) {
            fightAnimals();
            waitForEnter();
        } else if (choice == 2) {
            createDie();
            waitForEnter();
        } else if (choice == 3) {
            viewAnimalDice();
            waitForEnter();
        } else if (choice == 4) {
            viewMatchHistoryMenu();
            waitForEnter();
        } else if (choice == 0) {
            running = false;
        }
    }

    printDivider();
    std::cout << "Closing prototype.\n";
    return 0;
}
