#include <iostream>
#include <string_view>
#include <set>

#include "yaml/yaml_helper.h"
#include "seed_helper.h"
#include "helpers/terminal_format.h"


/// (has streak, in streak, streak abilities)
std::pair<std::set<Ability>, std::vector<bool>> findThreeStreaks(const std::vector<Ability>& rolls) {
    std::set<Ability> threeStreakAbilities{};
    std::vector<bool> inThreeStreak(rolls.size(), false);
    for (size_t i = 2; i < rolls.size(); i += 1) {
        if ((rolls[i - 2] == rolls[i]) && (rolls[i - 1] == rolls[i])) {
            threeStreakAbilities.insert(rolls[i]);
            inThreeStreak[i - 2] = true;
            inThreeStreak[i - 1] = true;
            inThreeStreak[i] = true;
        }
    }

    return std::make_pair(threeStreakAbilities, inThreeStreak);
}


/// Print and highlight 3-streaks.
void printRollsWithEmphasis(const std::string_view title, const std::vector<Ability>& rolls) {
    const auto [streakAbilities, inStreak] = findThreeStreaks(rolls);

    // Title.
    std::cout << TerminalFormat::BOLD << title << TerminalFormat::ENDC << "\n";
    if (!streakAbilities.empty()) {
        std::cout << "Streak abilities: ";
        for (const auto streakAbility: streakAbilities) {
            std::cout << TerminalFormat::BOLD << TerminalFormat::OK_GREEN << AbilityHelper::getId(streakAbility) << TerminalFormat::ENDC << " ";
        }
        std::cout << "\n";
    }

    // Rolls.
    for (size_t i = 0; i < rolls.size(); i += 1) {
        std::cout << i << ". ";

        const auto abilityId = AbilityHelper::getId(rolls[i]);
        if (inStreak[i]) {
            std::cout << TerminalFormat::BOLD << abilityId << TerminalFormat::ENDC << "\n";
        } else {
            std::cout << abilityId << "\n";
        }
    }
    std::cout << std::endl;
}


void printFutureRolls(std::string_view filename) {
    YamlFile yamlFile(filename);
    if (!yamlFile.getInitialSeed().has_value()) {
        throw std::runtime_error("No initial seed in file.");
    }

    SeedHelper seedHelper{yamlFile.getBrand()};
    const auto [valid, finalSeed] = seedHelper.advanceSeedToEndOfRollSequence(yamlFile.getInitialSeed().value(), yamlFile.getRollSequence());
    if (!valid) {
        throw std::runtime_error("The initial seed doesn't match the roll sequence.");
    }

    // Print gear information.
    std::cout << yamlFile.getName() << "\n";
    std::cout << "Brand: " << yamlFile.getBrand() << "\n";
    std::cout << std::hex << "Initial seed: 0x" << yamlFile.getInitialSeed().value() << " -> Final seed: 0x" << finalSeed << std::dec << std::endl;

    // No drink.
    const auto noDrinkResults = seedHelper.generateRolls(finalSeed);
    printRollsWithEmphasis("No drink:", noDrinkResults);

    // With drink.
    for (size_t i = 0; i < AbilityHelper::abilitiesCount; i += 1) {
        const auto drink = static_cast<Ability>(i);
        std::string drinkTitle{"Drink: "};
        drinkTitle += AbilityHelper::getId(drink);

        const auto withDrinkResults = seedHelper.generateRollsWithDrink(finalSeed, drink);
        printRollsWithEmphasis(drinkTitle, withDrinkResults);
    }
}


int main(int argc, char* argv[]) {
    // Parse arguments.
    if (argc == 1) {
        throw std::invalid_argument("No filename given.");
    }

    const auto filename = argv[1];

    printFutureRolls(filename);

    return 0;
}
