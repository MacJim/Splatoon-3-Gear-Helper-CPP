//
// Created by Jim Shi on 2022-11-08.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_ROLL_RANDOMIZER_H
#define SPLATOON_3_GEAR_HELPER_CPP_ROLL_RANDOMIZER_H

#include <chrono>
#include <random>

#include "../data/abilities.h"
#include "../data/roll_sequence.h"


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


#endif //SPLATOON_3_GEAR_HELPER_CPP_ROLL_RANDOMIZER_H
