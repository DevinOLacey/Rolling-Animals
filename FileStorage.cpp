#include "FileStorage.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace {

// Removes spaces from the start and end of a string.
std::string trim(const std::string& value) {
    std::size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

// Replaces runs of multiple spaces with a single space.
std::string collapseSpaces(const std::string& value) {
    std::ostringstream output;
    bool lastWasSpace = false;

    for (char current : value) {
        if (std::isspace(static_cast<unsigned char>(current))) {
            if (!lastWasSpace) {
                output << ' ';
            }
            lastWasSpace = true;
        } else {
            output << current;
            lastWasSpace = false;
        }
    }

    return output.str();
}

// Splits a string into pieces using one delimiter character.
std::vector<std::string> split(const std::string& text, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(text);
    std::string part;

    while (std::getline(stream, part, delimiter)) {
        parts.push_back(part);
    }

    return parts;
}

// Changes spaces in a name to underscores before saving it to a file.
std::string encodeName(const std::string& name) {
    std::string encoded = normalizeAnimalName(name);
    std::replace(encoded.begin(), encoded.end(), ' ', '_');
    return encoded;
}

// Changes underscores back to spaces after reading a name from a file.
std::string decodeName(const std::string& name) {
    std::string decoded = name;
    std::replace(decoded.begin(), decoded.end(), '_', ' ');
    return normalizeAnimalName(decoded);
}

// Finds the value for a key in a line of text.
// Example: in "name:Dog sides:1,2,3,4,5,6", asking for "name" returns "Dog".
std::string extractValue(const std::string& line, const std::string& key) {
    const std::string marker = key + ":";
    std::size_t start = line.find(marker);
    if (start == std::string::npos) {
        return "";
    }

    start += marker.size();
    std::size_t end = line.find(' ', start);
    if (end == std::string::npos) {
        end = line.size();
    }

    return line.substr(start, end - start);
}

// Creates a new animal record with all stats starting at 0.
AnimalRecord makeDefaultRecord(const std::string& name) {
    return AnimalRecord{name, 0, 0, 0, 0, 0, 0};
}

void validateAnimalDieData(const AnimalDieData& animal) {
    if (normalizeAnimalName(animal.name).empty()) {
        throw FileValidationException("Animal name cannot be empty.");
    }

    for (int sideValue : animal.sideValues) {
        if (sideValue < 1) {
            throw FileValidationException("Animal die side values must be 1 or greater.");
        }
    }
}

void validateSeriesRecord(const SeriesRecord& record) {
    if (normalizeAnimalName(record.animalA).empty() || normalizeAnimalName(record.animalB).empty()) {
        throw FileValidationException("A saved series must contain two animal names.");
    }

    if (namesMatch(record.animalA, record.animalB)) {
        throw FileValidationException("A series cannot use the same animal on both sides.");
    }

    if (record.games.empty()) {
        throw FileValidationException("A saved series must contain at least one counted game.");
    }

    if (!namesMatch(record.seriesWinner, record.animalA) && !namesMatch(record.seriesWinner, record.animalB)) {
        throw FileValidationException("Series winner must match one of the animals in the fight.");
    }
}

void validateAnimalRecordData(const AnimalRecord& record) {
    if (normalizeAnimalName(record.name).empty()) {
        throw FileValidationException("Animal stats record name cannot be empty.");
    }

    if (record.seriesWins < 0 || record.seriesLosses < 0 ||
        record.gameWins < 0 || record.gameLosses < 0 ||
        record.rollSum < 0 || record.rollCount < 0) {
        throw FileValidationException("Animal stats values cannot be negative.");
    }
}

// Finds an animal's record in the vector.
// If it does not exist yet, add a new empty record and return its index.
std::size_t ensureRecordIndex(std::vector<AnimalRecord>& records, const std::string& name) {
    const std::string normalizedName = normalizeAnimalName(name);

    for (std::size_t index = 0; index < records.size(); ++index) {
        if (namesMatch(records[index].name, normalizedName)) {
            return index;
        }
    }

    records.push_back(makeDefaultRecord(normalizedName));
    return records.size() - 1;
}

}

// Cleans up a name so it always has the same format:
// - removes extra spaces
// - fixes capitalization
// - keeps comparisons and saved names consistent
std::string normalizeAnimalName(const std::string& rawName) {
    std::string value = trim(collapseSpaces(rawName));
    bool capitalizeNext = true;

    for (char& current : value) {
        if (std::isspace(static_cast<unsigned char>(current))) {
            capitalizeNext = true;
            continue;
        }

        if (capitalizeNext) {
            current = static_cast<char>(std::toupper(static_cast<unsigned char>(current)));
            capitalizeNext = false;
        } else {
            current = static_cast<char>(std::tolower(static_cast<unsigned char>(current)));
        }
    }

    return value;
}

// Returns true if two names are the same after both are normalized.
bool namesMatch(const std::string& left, const std::string& right) {
    return normalizeAnimalName(left) == normalizeAnimalName(right);
}

// Reads all saved animal dice from a file and returns them.
// Bad or incomplete lines are skipped.
std::vector<AnimalDieData> loadAnimalDice(const std::string& filename) {
    std::vector<AnimalDieData> animals;
    std::ifstream file(filename);
    if (!file) {
        return animals;
    }
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        if (trim(line).empty()) {
            continue;
        }

        const std::string nameValue = extractValue(line, "name");
        const std::string sidesValue = extractValue(line, "sides");

        if (nameValue.empty() || sidesValue.empty()) {
            throw FileDataException("Invalid animal record in " + filename + " at line " + std::to_string(lineNumber) + ".");
        }

        std::vector<std::string> parts = split(sidesValue, ',');
        if (parts.size() != 6) {
            // Every animal die must have exactly 6 side values.
            throw FileDataException("Animal die record in " + filename + " line " + std::to_string(lineNumber) + " does not contain exactly 6 side values.");
        }

        AnimalDieData animal;
        animal.name = decodeName(nameValue);

        for (std::size_t index = 0; index < animal.sideValues.size(); ++index) {
            try {
                animal.sideValues[index] = std::stoi(parts[index]);
            } catch (const std::exception&) {
                // Skip this animal if any side value is not a valid number.
                throw FileDataException("Animal die record in " + filename + " line " + std::to_string(lineNumber) + " contains a side value that is not a valid number.");
            }
        }

        validateAnimalDieData(animal);
        animals.push_back(animal);
    }

    return animals;
}

// Checks whether an animal with this name is already in the list.
bool animalExists(const std::vector<AnimalDieData>& animals, const std::string& name) {
    for (const AnimalDieData& animal : animals) {
        if (namesMatch(animal.name, name)) {
            return true;
        }
    }
    return false;
}

// Adds one new animal die to the end of the animal file.
void appendAnimalDie(const std::string& filename, const AnimalDieData& animal) {
    validateAnimalDieData(animal);

    std::ofstream file(filename, std::ios::app);
    if (!file) {
        throw FileOpenException("Could not open " + filename + " for saving animal dice.");
    }

    file << "name:" << encodeName(animal.name) << " sides:";
    // Save the 6 side values as a comma-separated list.
    for (std::size_t index = 0; index < animal.sideValues.size(); ++index) {
        if (index > 0) {
            file << ",";
        }
        file << animal.sideValues[index];
    }
    file << "\n";

    if (!file) {
        throw FileOpenException("Could not finish writing the animal die to " + filename + ".");
    }

}

// Reads all finished series from the history file and rebuilds them in memory.
std::vector<SeriesRecord> loadSeriesHistory(const std::string& filename) {
    std::vector<SeriesRecord> records;
    std::ifstream file(filename);
    if (!file) {
        return records;
    }
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        if (trim(line).empty()) {
            continue;
        }

        SeriesRecord record;
        record.seriesWinner = decodeName(extractValue(line, "series_winner"));
        record.animalA = decodeName(extractValue(line, "animal_a"));
        record.animalB = decodeName(extractValue(line, "animal_b"));

        // The score is stored like "2-1", so split it at the dash.
        const std::string score = extractValue(line, "score");
        std::size_t dash = score.find('-');
        if (dash == std::string::npos) {
            throw FileDataException("Series record in " + filename + " line " + std::to_string(lineNumber) + " has an invalid score.");
        }

        try {
            record.animalAWins = std::stoi(score.substr(0, dash));
            record.animalBWins = std::stoi(score.substr(dash + 1));
        } catch (const std::exception&) {
            throw FileDataException("Series record in " + filename + " line " + std::to_string(lineNumber) + " contains a score that is not numeric.");
        }

        const std::string gamesValue = extractValue(line, "games");
        // The saved game list should be inside square brackets.
        if (gamesValue.size() < 2 || gamesValue.front() != '[' || gamesValue.back() != ']') {
            throw FileDataException("Series record in " + filename + " line " + std::to_string(lineNumber) + " is missing a valid game list.");
        }

        const std::string contents = gamesValue.substr(1, gamesValue.size() - 2);
        if (!contents.empty()) {
            std::vector<std::string> games = split(contents, ',');
            for (const std::string& gameText : games) {
                // Each saved game stores: winner name | winner roll | loser name | loser roll
                std::vector<std::string> parts = split(gameText, '|');
                if (parts.size() != 4) {
                    throw FileDataException("Series record in " + filename + " line " + std::to_string(lineNumber) + " contains an invalid saved game.");
                }

                try {
                    record.games.push_back(GameResult{
                        decodeName(parts[0]),
                        std::stoi(parts[1]),
                        decodeName(parts[2]),
                        std::stoi(parts[3])
                    });
                } catch (const std::exception&) {
                    throw FileDataException("Series record in " + filename + " line " + std::to_string(lineNumber) + " contains a game with invalid numeric values.");
                }
            }
        }

        validateSeriesRecord(record);
        records.push_back(record);
    }

    return records;
}

// Adds one finished series to the end of the history file.
void appendSeriesRecord(const std::string& filename, const SeriesRecord& record) {
    validateSeriesRecord(record);

    std::ofstream file(filename, std::ios::app);
    if (!file) {
        throw FileOpenException("Could not open " + filename + " for saving match history.");
    }

    file << "series_winner:" << encodeName(record.seriesWinner)
         << " animal_a:" << encodeName(record.animalA)
         << " animal_b:" << encodeName(record.animalB)
         << " score:" << record.animalAWins << "-" << record.animalBWins
         << " games:[";

    // Save each counted game in a compact text format.
    for (std::size_t index = 0; index < record.games.size(); ++index) {
        if (index > 0) {
            file << ",";
        }

        const GameResult& game = record.games[index];
        file << encodeName(game.winnerName) << "|"
             << game.winnerRoll << "|"
             << encodeName(game.loserName) << "|"
             << game.loserRoll;
    }

    file << "]\n";
    if (!file) {
        throw FileOpenException("Could not finish writing the match history to " + filename + ".");
    }
}

// Returns only the series that include the requested animal.
std::vector<SeriesRecord> filterSeriesByAnimal(
    const std::vector<SeriesRecord>& records,
    const std::string& animalName
) {
    std::vector<SeriesRecord> filtered;

    for (const SeriesRecord& record : records) {
        if (namesMatch(record.animalA, animalName) || namesMatch(record.animalB, animalName)) {
            filtered.push_back(record);
        }
    }

    return filtered;
}

// Reads the saved overall stats for each animal from the stats file.
std::vector<AnimalRecord> loadAnimalRecords(const std::string& filename) {
    std::vector<AnimalRecord> records;
    std::ifstream file(filename);
    if (!file) {
        return records;
    }
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        if (trim(line).empty()) {
            continue;
        }

        try {
            AnimalRecord record{
                decodeName(extractValue(line, "name")),
                std::stoi(extractValue(line, "series_wins")),
                std::stoi(extractValue(line, "series_losses")),
                std::stoi(extractValue(line, "game_wins")),
                std::stoi(extractValue(line, "game_losses")),
                std::stoi(extractValue(line, "roll_sum")),
                std::stoi(extractValue(line, "roll_count"))
            };
            validateAnimalRecordData(record);
            records.push_back(record);
        } catch (const FileValidationException&) {
            throw;
        } catch (const std::exception&) {
            // Skip lines that do not contain valid numeric stat values.
            throw FileDataException("Animal stats record in " + filename + " line " + std::to_string(lineNumber) + " contains invalid numeric values.");
        }
    }

    return records;
}

// Rewrites the full stats file using the records currently in memory.
void saveAnimalRecords(const std::string& filename, const std::vector<AnimalRecord>& records) {
    std::ofstream file(filename);
    if (!file) {
        throw FileOpenException("Could not open " + filename + " for saving animal records.");
    }

    for (const AnimalRecord& record : records) {
        validateAnimalRecordData(record);
        file << "name:" << encodeName(record.name)
             << " series_wins:" << record.seriesWins
             << " series_losses:" << record.seriesLosses
             << " game_wins:" << record.gameWins
             << " game_losses:" << record.gameLosses
             << " roll_sum:" << record.rollSum
             << " roll_count:" << record.rollCount
             << "\n";
    }

    if (!file) {
        throw FileOpenException("Could not finish writing animal records to " + filename + ".");
    }

}

// Finds an animal record by name and returns a pointer to it.
// Returns nullptr if the animal is not found.
AnimalRecord* findAnimalRecord(std::vector<AnimalRecord>& records, const std::string& name) {
    for (AnimalRecord& record : records) {
        if (namesMatch(record.name, name)) {
            return &record;
        }
    }
    return nullptr;
}

// Same lookup as above, but for read-only vectors.
const AnimalRecord* findAnimalRecord(const std::vector<AnimalRecord>& records, const std::string& name) {
    for (const AnimalRecord& record : records) {
        if (namesMatch(record.name, name)) {
            return &record;
        }
    }
    return nullptr;
}

// Updates the overall stats file after one finished best-of-3 series.
// This includes:
// - series wins and losses
// - individual game wins and losses
// - total roll sum and roll count for average-roll calculations
void updateAnimalRecords(const std::string& filename, const SeriesRecord& record) {
    validateSeriesRecord(record);
    std::vector<AnimalRecord> records = loadAnimalRecords(filename);

    const std::size_t animalAIndex = ensureRecordIndex(records, record.animalA);
    const std::size_t animalBIndex = ensureRecordIndex(records, record.animalB);

    // Update the best-of-3 series result.
    if (namesMatch(record.seriesWinner, record.animalA)) {
        ++records[animalAIndex].seriesWins;
        ++records[animalBIndex].seriesLosses;
    } else {
        ++records[animalBIndex].seriesWins;
        ++records[animalAIndex].seriesLosses;
    }

    // Update the per-game stats using each counted game in the series.
    for (const GameResult& game : record.games) {
        const std::size_t winnerIndex = ensureRecordIndex(records, game.winnerName);
        const std::size_t loserIndex = ensureRecordIndex(records, game.loserName);

        ++records[winnerIndex].gameWins;
        ++records[loserIndex].gameLosses;
        records[winnerIndex].rollSum += game.winnerRoll;
        records[winnerIndex].rollCount += 1;
        records[loserIndex].rollSum += game.loserRoll;
        records[loserIndex].rollCount += 1;
    }

    // Sort records by name so the file stays easy to read.
    std::sort(records.begin(), records.end(), [](const AnimalRecord& left, const AnimalRecord& right) {
        return left.name < right.name;
    });

    saveAnimalRecords(filename, records);
}

// Calculates the average of all recorded battle rolls for one animal.
// This is not the average of the die's 6 face values.
double computeAverageRoll(const AnimalRecord& record) {
    if (record.rollCount == 0) {
        return 0.0;
    }

    return static_cast<double>(record.rollSum) / static_cast<double>(record.rollCount);
}
