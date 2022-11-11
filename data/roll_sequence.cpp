//
// Created by Jim Shi on 2022-11-03.
//

#include "roll_sequence.h"


RollSequence::RollSequence(const std::vector<Ability> &rolls): data{} {
    data.reserve(rolls.size());

    for (const auto roll: rolls) {
        data.emplace_back(roll, Ability::noDrink);
    }
}

std::unordered_set<Ability> RollSequence::getDrinksUsed() const {
     std::unordered_set<Ability> returnValue{};
     for (const auto [roll, drink]: data) {
         if (drink != Ability::noDrink) {
             returnValue.insert(drink);
         }
     }

     return returnValue;
}
