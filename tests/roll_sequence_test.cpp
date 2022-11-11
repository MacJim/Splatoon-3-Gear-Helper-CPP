//
// Created by Jim Shi on 2022-11-03.
//

#include "gtest/gtest.h"

#include "roll_randomizer.h"


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
            EXPECT_EQ(it2->second, Ability::noDrink) << testCaseDescription;
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
            EXPECT_EQ(it2->second, Ability::noDrink) << testCaseDescription;
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
        std::make_pair(static_cast<Ability>(0), static_cast<Ability>(1)),
        std::make_pair(static_cast<Ability>(2), static_cast<Ability>(2)),
        std::make_pair(static_cast<Ability>(3), Ability::noDrink),
        std::make_pair(static_cast<Ability>(5), static_cast<Ability>(6)),
    };
    const std::unordered_set expectedResult{static_cast<Ability>(1), static_cast<Ability>(2), static_cast<Ability>(6)};

    RollSequence seq{};

    for (const auto& [roll, drink]: rollsAndDrinks) {
        if (drink == Ability::noDrink) {
            seq.addRoll(roll);
        } else {
            seq.addRoll(roll, drink);
        }
    }

    EXPECT_EQ(seq.getDrinksUsed(), expectedResult);
}
