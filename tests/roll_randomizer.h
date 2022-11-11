//
// Created by Jim Shi on 2022-11-08.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_ROLL_RANDOMIZER_H
#define SPLATOON_3_GEAR_HELPER_CPP_ROLL_RANDOMIZER_H

#include <chrono>
#include <random>

#include "../data/ability.h"
#include "../data/roll_sequence.h"


/// Get a random ability that's not placeholder/noDrink.
Ability getRandomAbility();

/// Get a list of random abilities that are not placeholder/noDrink.
std::vector<Ability> getRandomRolls(size_t length);

/**
 * Get a list of random rolls and drinks.
 *
 * Each drink has a chance of being `noDrink`.
 */
RollSequence::DataType getRandomRollsAndDrinks(size_t length);


#endif //SPLATOON_3_GEAR_HELPER_CPP_ROLL_RANDOMIZER_H
