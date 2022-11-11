//
// Created by Jim Shi on 2022-11-10.
//

#include "roll_randomizer.h"


Ability getRandomAbility() {
    static auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    static std::uniform_int_distribution<size_t> uniformDistribution(0, AbilityHelper::ids.size() - 1);
    static std::default_random_engine generator(seed);

    return static_cast<Ability>(uniformDistribution(generator));
}


std::vector<Ability> getRandomRolls(size_t length) {
    std::vector<Ability> returnValue(length, Ability::unknown);
    for (size_t i = 0; i < length; i += 1) {
        returnValue[i] = getRandomAbility();
    }

    return returnValue;
}


RollSequence::DataType getRandomRollsAndDrinks(size_t length) {
    RollSequence::DataType returnValue{};
    returnValue.reserve(length);

    for (size_t i = 0; i < length; i += 1) {
        bool noDrink = (getRandomAbility() == Ability::inkSaverMain);
        returnValue.emplace_back(getRandomAbility(), noDrink ? Ability::noDrink : getRandomAbility());
    }

    return returnValue;
}
