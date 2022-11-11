//
// Created by Jim Shi on 2022-11-03.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_ROLLSEQUENCE_H
#define SPLATOON_3_GEAR_HELPER_CPP_ROLLSEQUENCE_H


#include <vector>
#include <unordered_set>
#include <string_view>

#include "ability.h"


class RollSequence {
public:
    /**
     * {(rolled ability, drink)}
     */
    using DataType = std::vector<std::pair<Ability, Ability>>;
private:
    DataType data;

public:
    inline DataType::iterator begin() {
        return data.begin();
    }
    inline DataType::const_iterator begin() const {
        return data.begin();
    }
    inline DataType::iterator end() {
        return data.end();
    }
    inline DataType::const_iterator end() const {
        return data.end();
    }
    inline DataType::size_type size() const {
        return data.size();
    }
    inline bool empty() const {
        return data.empty();
    }

public:
    /// Empty sequence.
    RollSequence(): data{} {};

    /**
     * Sequence with no drink.
     *
     * Currently only used in tests.
     */
    explicit RollSequence(const std::vector<Ability>& rolls);

public:
    inline void addRoll(const Ability ability) {
        data.emplace_back(ability, Ability::noDrink);
    }
    inline void addRoll(const Ability ability, const Ability drink) {
        data.emplace_back(ability, drink);
    }

public:
    /**
     * Get all drinks used.
     *
     * Used to calculate/cache the "weights map" for each type of drink.
     */
    [[nodiscard]] std::unordered_set<Ability> getDrinksUsed() const;
};


#endif //SPLATOON_3_GEAR_HELPER_CPP_ROLLSEQUENCE_H
