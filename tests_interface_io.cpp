#include "FileStorage.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

namespace {

// Stops the test program right away if a check fails.
void require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "TEST FAILURE: " << message << "\n";
        std::exit(1);
    }
}

// Makes sure animal names are cleaned up the same way every time.
void testNameNormalization() {
    require(normalizeAnimalName("dog") == "Dog", "lowercase names should normalize to Title Case");
    require(normalizeAnimalName(" DOG ") == "Dog", "trimmed uppercase names should normalize");
    require(normalizeAnimalName("dOg") == "Dog", "mixed-case names should normalize");
    require(normalizeAnimalName("snow   leopard") == "Snow Leopard", "internal spaces should collapse");
}

// Tests saving one animal, loading it back, and checking for duplicate names.
void testAnimalSaveLoadAndDuplicates() {
    const std::string animalFile = "test_animals.txt";
    std::remove(animalFile.c_str());

    AnimalDieData dog{{}, {3, 3, 3, 4, 5, 6}};
    dog.name = "dog";
    require(appendAnimalDie(animalFile, dog), "animal file should append");

    std::vector<AnimalDieData> animals = loadAnimalDice(animalFile);
    require(animals.size() == 1, "one animal should load");
    require(animals[0].name == "Dog", "loaded animal name should be normalized");
    require(animalExists(animals, " DOG "), "duplicate lookup should be case-insensitive");

    std::remove(animalFile.c_str());
}

// Tests that animal stats update correctly after finished series
// and that average roll is based on real recorded rolls.
void testAnimalStatsUpdates() {
    const std::string statsFile = "test_stats.txt";
    std::remove(statsFile.c_str());

    SeriesRecord record;
    record.seriesWinner = "Dog";
    record.animalA = "Dog";
    record.animalB = "Cat";
    record.animalAWins = 2;
    record.animalBWins = 1;
    record.games = {
        {"Dog", 6, "Cat", 2},
        {"Cat", 5, "Dog", 4},
        {"Dog", 6, "Cat", 1}
    };

    require(updateAnimalRecords(statsFile, record), "stats should update for a series");

    std::vector<AnimalRecord> records = loadAnimalRecords(statsFile);
    const AnimalRecord* dog = findAnimalRecord(records, "dog");
    const AnimalRecord* cat = findAnimalRecord(records, "cat");

    require(dog != nullptr, "dog stats should exist");
    require(cat != nullptr, "cat stats should exist");
    require(dog->seriesWins == 1 && dog->seriesLosses == 0, "dog should be undefeated in series");
    require(dog->gameWins == 2 && dog->gameLosses == 1, "dog should track individual games");
    require(std::fabs(computeAverageRoll(*dog) - (16.0 / 3.0)) < 0.0001, "dog average should use actual rolls");
    require(cat->gameWins == 1 && cat->gameLosses == 2, "cat games should update too");

    SeriesRecord followUp;
    followUp.seriesWinner = "Dog";
    followUp.animalA = "Dog";
    followUp.animalB = "Fox";
    followUp.animalAWins = 2;
    followUp.animalBWins = 1;
    followUp.games = {
        {"Dog", 2, "Fox", 1},
        {"Fox", 6, "Dog", 4},
        {"Dog", 4, "Fox", 3}
    };

    require(updateAnimalRecords(statsFile, followUp), "stats should support multiple series");
    records = loadAnimalRecords(statsFile);
    dog = findAnimalRecord(records, "Dog");
    require(dog != nullptr, "dog stats should still exist");
    require(std::fabs(computeAverageRoll(*dog) - (26.0 / 6.0)) < 0.0001, "average should update from total rolls");

    std::remove(statsFile.c_str());
}

// Tests saving match history, loading it back, and filtering it by animal name.
void testHistoryAppendLoadAndFilter() {
    const std::string historyFile = "test_history.txt";
    std::remove(historyFile.c_str());

    SeriesRecord dogVsCat;
    dogVsCat.seriesWinner = "Dog";
    dogVsCat.animalA = "Dog";
    dogVsCat.animalB = "Cat";
    dogVsCat.animalAWins = 2;
    dogVsCat.animalBWins = 1;
    dogVsCat.games = {
        {"Dog", 6, "Cat", 1},
        {"Cat", 5, "Dog", 2},
        {"Dog", 4, "Cat", 3}
    };

    SeriesRecord foxVsOwl;
    foxVsOwl.seriesWinner = "Fox";
    foxVsOwl.animalA = "Fox";
    foxVsOwl.animalB = "Owl";
    foxVsOwl.animalAWins = 2;
    foxVsOwl.animalBWins = 0;
    foxVsOwl.games = {
        {"Fox", 6, "Owl", 2},
        {"Fox", 3, "Owl", 1}
    };

    require(appendSeriesRecord(historyFile, dogVsCat), "history append should succeed");
    require(appendSeriesRecord(historyFile, foxVsOwl), "second history append should succeed");

    std::vector<SeriesRecord> records = loadSeriesHistory(historyFile);
    require(records.size() == 2, "two series should load");

    std::vector<SeriesRecord> dogMatches = filterSeriesByAnimal(records, " dog ");
    require(dogMatches.size() == 1, "filter should only return matching series");
    require(namesMatch(dogMatches[0].seriesWinner, "Dog"), "filtered record should be the dog series");

    std::vector<SeriesRecord> owlMatches = filterSeriesByAnimal(records, "Owl");
    require(owlMatches.size() == 1, "owl should only appear once");

    std::remove(historyFile.c_str());
}

}

// Runs all tests. If no test fails, print a success message.
int main() {
    testNameNormalization();
    testAnimalSaveLoadAndDuplicates();
    testAnimalStatsUpdates();
    testHistoryAppendLoadAndFilter();

    std::cout << "All interface/file I/O tests passed.\n";
    return 0;
}
