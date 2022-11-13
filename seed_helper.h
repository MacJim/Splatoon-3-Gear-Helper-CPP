//
// Created by Jim Shi on 2022-10-28.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_SEED_HELPER_H
#define SPLATOON_3_GEAR_HELPER_CPP_SEED_HELPER_H

#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

#include "data/roll_sequence.h"


class SeedHelper {
#pragma mark Constructor
public:
    explicit SeedHelper(std::string_view brandName);

public:
    std::string brandName;

#pragma mark Brand weight, no drink
private:
    /// Roll mod.
    uint32_t totalWeight;
    /**
     * Each ability's weight (without drink).
     * Indices correspond to `Ability`'s values and `AbilityHelper::ids`.
     *
     * - Used to calculate `rollToAbilityMap`
     * - Not used for calculating abilities
     * - May be used for calculating `drinkRollToAbilityMap` if drinks are provided in `findSeed`'s roll sequence
     */
    std::array<uint32_t, AbilityHelper::abilitiesCount> cachedWeights;
    /**
     * Roll to ability map (without drink).
     * Calculated once upon class construction.
     */
    std::vector<Ability> rollToAbilityMap;

#pragma mark Brand weights, with drinks
private:
    /**
     * (total weight, roll to ability map)
     *
     * Indices: Drink used: Correspond to `Ability`'s values and `AbilityHelper::ids`.
     */
    std::array<std::pair<uint32_t, std::vector<Ability>>, AbilityHelper::abilitiesCount> drinkRollToAbilityMap;

public:
    void cacheDrinkRollToAbilityMap(Ability drink);

    /// Cache `drinkRollToAbilityMap` for all drink types.
    void cacheAllDrinkRollToAbilityMaps();

#pragma mark Seed
public:
    static uint32_t advanceSeed(uint32_t seed);

    /**
     * Advance from `initialSeed` to the end of the roll sequence.
     * The roll sequence is verified against the initial initialSeed.
     *
     * Used in predictions.
     *
     * @return (initialSeed validity, seed after the roll sequence)
     */
    [[nodiscard]] std::pair<bool, uint32_t> advanceSeedToEndOfRollSequence(uint32_t initialSeed, const RollSequence& rollSequence);

#pragma mark Roll abilities
public:
    [[nodiscard]] Ability getBrandedAbility(uint32_t seed) const;

    [[nodiscard]] Ability getBrandedAbilityWithDrink(uint32_t seed, Ability drink) const;

public:
    /**
     * Roll once and generate the next ability.
     * @return (next seed, next ability)
     */
    [[nodiscard]] std::pair<uint32_t, Ability> generateRoll(uint32_t seed) const;

    [[nodiscard]] std::pair<uint32_t, Ability> generateRollWithDrink(uint32_t seed, Ability drink) const;

#pragma mark Find seed
private:
    /// Find valid seeds in the range [seedStart, seedStop].
    [[nodiscard]] std::vector<uint32_t> findSeedWorker(const RollSequence& previousRolls, uint32_t seedStart, uint32_t seedStop) const;

public:
    std::vector<uint32_t> findSeed(const RollSequence& previousRolls, size_t workersCount = 0);
};


#endif //SPLATOON_3_GEAR_HELPER_CPP_SEED_HELPER_H
