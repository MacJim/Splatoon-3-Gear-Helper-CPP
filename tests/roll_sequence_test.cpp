//
// Created by Jim Shi on 2022-11-03.
//

#include <algorithm>
#include <chrono>
#include <random>

#include "gtest/gtest.h"

#include "../data/roll_sequence.h"
#include "../data/abilities.h"


#pragma mark Random helpers
std::string_view getRandomAbility() {
    static auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    static std::uniform_int_distribution<size_t> uniformDistribution(0, abilities.size() - 1);
    static std::default_random_engine generator(seed);

    return abilities[uniformDistribution(generator)];
}


std::vector<std::string_view> getRandomRolls(size_t length) {
    std::vector<std::string_view> returnValue(length, "");
    for (size_t i = 0; i < length; i += 1) {
        returnValue[i] = getRandomAbility();
    }

    return returnValue;
}


RollSequence::DataType getRandomRollsAndDrinks(size_t length) {
    RollSequence::DataType returnValue{};

    returnValue.reserve(length);
    for (size_t i = 0; i < length; i += 1) {
        bool noDrink = (getRandomAbility() == abilities[0]);
        returnValue.emplace_back(getRandomAbility(), noDrink ? "" : getRandomAbility());
    }

    return returnValue;
}


#pragma mark Tests
TEST(RollSequenceTest, ConstructFromSequence) {
    for (size_t length = 0; length <= 10000; length += 1) {
        const auto rolls = getRandomRolls(length);
        std::string testCaseDescription = "Length: " + std::to_string(length) + "; Test case: " + ::testing::PrintToString(rolls);

        RollSequence seq(rolls);

        EXPECT_EQ(rolls.size(), seq.size()) << testCaseDescription;

        auto it1 = rolls.begin();
        auto it2 = seq.begin();
        while (it1 != rolls.end()) {
            EXPECT_EQ(*it1, it2->first) << testCaseDescription;
            EXPECT_EQ(it2->second, "") << testCaseDescription;
            it1 += 1;
            it2 += 1;
        }
    }
}


TEST(RollSequenceTest, AddRollNoDrink) {
    for (size_t length = 0; length <= 10000; length += 1) {
        const auto rolls = getRandomRolls(length);
        std::string testCaseDescription = "Length: " + std::to_string(length) + "; Test case: " + ::testing::PrintToString(rolls);

        RollSequence seq{};

        for (size_t i = 0; i < length; i += 1) {
            seq.addRoll(rolls[i]);
        }

        EXPECT_EQ(rolls.size(), seq.size()) << testCaseDescription;

        auto it1 = rolls.begin();
        auto it2 = seq.begin();
        while (it1 != rolls.end()) {
            EXPECT_EQ(*it1, it2->first) << testCaseDescription;
            EXPECT_EQ(it2->second, "") << testCaseDescription;
            it1 += 1;
            it2 += 1;
        }
    }
}


TEST(RollSequenceTest, AddRollWithDrink) {
    for (size_t length = 0; length < 10000; length += 1) {
        const auto rollsAndDrinks = getRandomRollsAndDrinks(length);
        std::string testCaseDescription = "Length: " + std::to_string(length) + "; Test case: " + ::testing::PrintToString(rollsAndDrinks);

        RollSequence seq{};

        for (const auto& [roll, drink]: rollsAndDrinks) {
            seq.addRoll(roll, drink);
        }

        EXPECT_EQ(rollsAndDrinks.size(), seq.size()) << testCaseDescription;

        auto it1 = rollsAndDrinks.begin();
        auto it2 = seq.begin();
        while (it1 != rollsAndDrinks.end()) {
            EXPECT_EQ(it1->first, it2->first) << testCaseDescription;
            EXPECT_EQ(it1->second, it2->second) << testCaseDescription;
            it1 += 1;
            it2 += 1;
        }
    }
}


TEST(RollSequenceTest, GetUsedDrinks) {
    const RollSequence::DataType rollsAndDrinks{
        std::make_pair(abilities[0], abilities[1]),
        std::make_pair(abilities[2], abilities[2]),
        std::make_pair(abilities[3], ""),
        std::make_pair(abilities[5], abilities[6]),
    };
    const std::unordered_set expectedResult{abilities[1], abilities[2], abilities[6]};

    RollSequence seq{};

    for (const auto& [roll, drink]: rollsAndDrinks) {
        if (drink.empty()) {
            seq.addRoll(roll);
        } else {
            seq.addRoll(roll, drink);
        }
    }

    EXPECT_EQ(seq.getDrinksUsed(), expectedResult);
}
