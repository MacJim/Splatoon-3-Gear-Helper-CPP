//
// Created by Jim Shi on 2022-11-03.
//

#include "roll_sequence.h"


RollSequence::RollSequence(const std::vector<std::string_view> &rolls): data{} {
    data.reserve(rolls.size());

    for (const auto& roll: rolls) {
        data.emplace_back(roll, "");
    }
}

std::unordered_set<std::string_view> RollSequence::getDrinksUsed() {
     std::unordered_set<std::string_view> returnValue{};
     for (const auto& [roll, drink]: data) {
         if (!drink.empty()) {
             returnValue.insert(drink);
         }
     }

     return returnValue;
}
