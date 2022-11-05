//
// Created by Jim Shi on 2022-10-28.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_SEED_HELPER_H
#define SPLATOON_3_GEAR_HELPER_CPP_SEED_HELPER_H

#include <cstdint>
#include <vector>
#include <string_view>
#include <unordered_map>

#include "data/abilities.h"
#include "data/roll_sequence.h"


class SeedHelper {
#pragma mark Constructor
public:
    explicit SeedHelper(std::string_view brandName);

#pragma mark Brand weight
public:
    std::string_view brandName;

private:  // No drink.
    /// Roll mod.
    uint32_t totalWeight;
    /**
     * Each ability's weight (without drink).
     * Indices correspond to `abilities`.
     *
     * - Not used for calculating abilities
     * - May be used for calculating `cachedDrinkWeightsMap`
     */
    std::array<uint32_t, abilities.size()> cachedWeights;
    /**
     * Weight to ability ID map.
     * Calculated once upon class construction.
     */
    std::vector<std::string_view> cachedWeightsMap;

private:  // With drink.
    /**
     * (total weight, weight to ability ID map)
     *
     * Indices correspond to `abilities`.
     */
    std::array<std::pair<uint32_t, std::vector<std::string_view>>, abilities.size()> cachedDrinkWeightsMap;
    void cacheDrinkWeightMap(const std::string_view& drink);

#pragma mark Seed
public:
    static uint32_t advanceSeed(uint32_t seed);

#pragma mark Roll abilities
public:
    std::string_view getBrandedAbility(uint32_t seed) const;

    std::string_view getBrandedAbilityWithDrink(uint32_t seed, const std::string_view& drink) const;

public:
    /**
     * Roll once and generate the next ability.
     * @return (next seed, next ability)
     */
    std::pair<uint32_t, std::string_view> generateRoll(uint32_t seed) const;

    std::pair<uint32_t, std::string_view> generateRollWithDrink(uint32_t seed, const std::string_view& drink) const;

#pragma mark Find seed
public:
    std::vector<uint32_t> findSeed(const RollSequence& previousRolls);
};


#endif //SPLATOON_3_GEAR_HELPER_CPP_SEED_HELPER_H
