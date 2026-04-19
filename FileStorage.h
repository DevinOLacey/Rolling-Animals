#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

// Used for saved dice, match history, and animal stats.

#include <array>
#include <stdexcept>
#include <string>
#include <vector>

class FileStorageException : public std::runtime_error {
public:
    explicit FileStorageException(const std::string& message) : std::runtime_error(message) {}
};

class FileOpenException : public FileStorageException {
public:
    explicit FileOpenException(const std::string& message) : FileStorageException(message) {}
};

class FileDataException : public FileStorageException {
public:
    explicit FileDataException(const std::string& message) : FileStorageException(message) {}
};

class FileValidationException : public FileStorageException {
public:
    explicit FileValidationException(const std::string& message) : FileStorageException(message) {}
};

// Stores one animal die.
// Each animal has a name and exactly 6 side values.
struct AnimalDieData {
    std::string name;
    std::array<int, 6> sideValues;
};

// Stores the result of one counted game inside a series.
// It keeps the winner, loser, and the roll each one got.
struct GameResult {
    std::string winnerName;
    int winnerRoll;
    std::string loserName;
    int loserRoll;
};

// Stores one finished best-of-3 match between two animals.
// It includes the final series score and the list of counted games.
struct SeriesRecord {
    std::string seriesWinner;
    std::string animalA;
    std::string animalB;
    int animalAWins;
    int animalBWins;
    std::vector<GameResult> games;
};

// Stores the long-term stats for one animal across all saved fights.
struct AnimalRecord {
    std::string name;
    int seriesWins;
    int seriesLosses;
    int gameWins;
    int gameLosses;
    int rollSum;    // Total of all recorded rolls for this animal
    int rollCount;  // Number of recorded rolls for this animal
};

// Cleans up a name so comparisons stay consistent.
// Example: "  snow   leopard " becomes "Snow Leopard".
// Compares two names after normalizing them first.
std::string normalizeAnimalName(const std::string& rawName);
bool namesMatch(const std::string& left, const std::string& right);

// Functions for reading and writing saved animal dice.
std::vector<AnimalDieData> loadAnimalDice(const std::string& filename);
bool animalExists(const std::vector<AnimalDieData>& animals, const std::string& name);
void appendAnimalDie(const std::string& filename, const AnimalDieData& animal);

// Functions for reading, saving, and filtering completed match history.
std::vector<SeriesRecord> loadSeriesHistory(const std::string& filename);
void appendSeriesRecord(const std::string& filename, const SeriesRecord& record);
std::vector<SeriesRecord> filterSeriesByAnimal(
    const std::vector<SeriesRecord>& records,
    const std::string& animalName
);

// Functions for reading, saving, and updating overall animal stats.
std::vector<AnimalRecord> loadAnimalRecords(const std::string& filename);
void saveAnimalRecords(const std::string& filename, const std::vector<AnimalRecord>& records);
void updateAnimalRecords(const std::string& filename, const SeriesRecord& record);
AnimalRecord* findAnimalRecord(std::vector<AnimalRecord>& records, const std::string& name);
const AnimalRecord* findAnimalRecord(const std::vector<AnimalRecord>& records, const std::string& name);
double computeAverageRoll(const AnimalRecord& record);

#endif
