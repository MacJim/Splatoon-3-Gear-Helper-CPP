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

public:
    std::string_view brandName;

#pragma mark Brand weight, no drink
private:
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

#pragma mark Brand weights, with drinks
private:
    /**
     * (total weight, weight to ability ID map)
     *
     * Indices correspond to `abilities`.
     */
    std::array<std::pair<uint32_t, std::vector<std::string_view>>, abilities.size()> cachedDrinkWeightsMap;

public:
    void cacheDrinkWeightsMap(const std::string_view& drink);

    /// Cache `cachedDrinkWeightsMap` for all drink types.
    void cacheAllDrinkWeightsMaps();

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
private:
    /// Find valid seeds in the range [seedStart, seedStop].
    std::vector<uint32_t> findSeedWorker(const RollSequence& previousRolls, uint32_t seedStart, uint32_t seedStop) const;

public:
    std::vector<uint32_t> findSeed(const RollSequence& previousRolls, size_t workersCount = 0);
};


#endif //SPLATOON_3_GEAR_HELPER_CPP_SEED_HELPER_H
