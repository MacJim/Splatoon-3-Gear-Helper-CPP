//
// Created by Jim Shi on 2022-11-03.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_ROLLSEQUENCE_H
#define SPLATOON_3_GEAR_HELPER_CPP_ROLLSEQUENCE_H


#include <vector>
#include <unordered_set>
#include <string_view>


class RollSequence {
public:
    using DataType = std::vector<std::pair<std::string_view, std::string_view>>;
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
    explicit RollSequence(const std::vector<std::string_view>& rolls);

public:
    inline void addRoll(const std::string_view& ability) {
        data.emplace_back(ability, "");
    }
    inline void addRoll(const std::string_view& ability, const std::string_view& drink) {
        data.emplace_back(ability, drink);
    }

public:
    /**
     * Get all drinks used.
     *
     * Used to calculate/cache the "weights map" for each type of drink.
     */
    [[nodiscard]] std::unordered_set<std::string_view> getDrinksUsed() const;
};


#endif //SPLATOON_3_GEAR_HELPER_CPP_ROLLSEQUENCE_H
