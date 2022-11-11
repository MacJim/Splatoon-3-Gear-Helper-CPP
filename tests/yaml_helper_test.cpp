//
// Created by Jim Shi on 2022-11-08.
//


#include <filesystem>
#include <iostream>
#include <stdexcept>

#include "gtest/gtest.h"
#include "../yaml/yaml_helper.h"

#include "roll_randomizer.h"


#pragma mark Helpers
/// RAII
class TemporaryFile {
public:
    std::filesystem::path path;

    [[nodiscard]] std::string getFilename() const {
        return path.string();
    }

    // Deleted constructors.
    TemporaryFile() = delete;
    TemporaryFile(const TemporaryFile&) = delete;
    TemporaryFile& operator=(const TemporaryFile&) = delete;

    /**
     * Get the filename of the specified temporary file if it doesn't exist.
     */
    explicit TemporaryFile(std::string_view filename) {
        if (filename.empty()) {
            throw std::invalid_argument("Filename is empty.");
        }

        path = std::filesystem::path(::testing::TempDir());
        if (!std::filesystem::is_directory(path)) {
            std::string errorMessage{"Temporary dir `"};
            errorMessage += path;
            errorMessage += "` doesn't exist!";
            throw std::runtime_error(errorMessage);
        }

        path /= filename;
        if (std::filesystem::exists(path)) {
            std::string errorMessage{path};
            errorMessage += " already exists!";
            throw std::runtime_error(errorMessage);
        } else {
            // NOTE: I should actually create the file here (which involves using a mutex).
            // But since this class isn't multi-threaded, I'll just omit this part 😂.
            std::cout << "RAII temporary file: " << getFilename() << std::endl;
        }
    }

    ~TemporaryFile() {
        // Check if `path` is indeed initialized before deleting it.
        // TODO: Is this check really needed? The destructor won't get called if the constructor failed.
        if ((!path.empty()) && (std::filesystem::is_regular_file(path))) {
            // In fact, I'm too afraid to delete a file. So I'll just rename it here instead.
            auto newPath = path;
            newPath += ".old";  // Hmm although `filesystem::path` has `/`, it doesn't support `+`.
            std::filesystem::rename(path, newPath);
        }
    }
};


#pragma mark Tests
TEST(YamlHelperTest, CreateSaveLoadNoDrink) {
    TemporaryFile temporaryFile{"splatoon_no_drink.yaml"};
    const auto filename = temporaryFile.getFilename();
    std::string testCaseDescription = "Temporary file: " + filename;

    // Create and save.
    const std::string name{"Crimson Parashooter"};
    const std::string brand{"Annaki"};
    auto rolls = getRandomRolls(100);

    {
        YamlFile yamlFile{filename, name, brand, {}};
        for (const auto roll: rolls) {
            yamlFile.addRoll(roll);
        }
        // The file should be saved by `YamlFile`'s destructor.
    }

    ASSERT_TRUE(std::filesystem::is_regular_file(temporaryFile.path)) << testCaseDescription;

    // Load.
    YamlFile yamlFile{filename};
    EXPECT_EQ(yamlFile.getName(), name) << testCaseDescription;
    EXPECT_EQ(yamlFile.getBrand(), brand) << testCaseDescription;
    EXPECT_FALSE(yamlFile.getInitialSeed().has_value()) << testCaseDescription;

    auto it1 = rolls.begin();
    auto it2 = yamlFile.getRollSequence().begin();
    while (it1 != rolls.end()) {
        EXPECT_EQ(*it1, it2->first) << testCaseDescription;
        EXPECT_EQ(it2->second, Ability::noDrink) << testCaseDescription;

        it1 += 1;
        it2 += 1;
    }
}


TEST(YamlHelperTest, CreateSaveLoadWithDrink) {
    TemporaryFile temporaryFile{"splatoon_with_drink.yaml"};
    const auto filename = temporaryFile.getFilename();
    std::string testCaseDescription = "Temporary file: " + filename;

    // Create and save.
    const std::string name{"Wasabi Tabi"};
    const std::string brand{"Tentatek"};
    const uint32_t initialSeed = UINT32_MAX - 1;
    auto rollsAndDrinks = getRandomRollsAndDrinks(100);

    {
        YamlFile yamlFile{filename, name, brand, initialSeed};
        for (const auto [roll, drink]: rollsAndDrinks) {
            yamlFile.addRoll(roll, drink);
        }
        // The file should be saved by `YamlFile`'s destructor.
    }

    ASSERT_TRUE(std::filesystem::is_regular_file(temporaryFile.path)) << testCaseDescription;

    // Load.
    YamlFile yamlFile{filename};
    EXPECT_EQ(yamlFile.getName(), name) << testCaseDescription;
    EXPECT_EQ(yamlFile.getBrand(), brand) << testCaseDescription;
    EXPECT_TRUE(yamlFile.getInitialSeed().has_value()) << testCaseDescription;
    EXPECT_EQ(yamlFile.getInitialSeed().value(), initialSeed) << testCaseDescription;

    auto it1 = rollsAndDrinks.begin();
    auto it2 = yamlFile.getRollSequence().begin();
    while (it1 != rollsAndDrinks.end()) {
        EXPECT_EQ(it1->first, it2->first) << testCaseDescription;
        EXPECT_EQ(it1->second, it2->second) << testCaseDescription;

        it1 += 1;
        it2 += 1;
    }
}
