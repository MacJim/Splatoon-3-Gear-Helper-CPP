//
// Created by Jim Shi on 2022-10-28.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_SEED_HELPER_H
#define SPLATOON_3_GEAR_HELPER_CPP_SEED_HELPER_H

#include <cstdint>
#include <vector>
#include <string_view>


class SeedHelper {
#pragma mark Constructor
public:
    SeedHelper(std::string_view brandName);

#pragma mark Brand weight
public:
    std::string_view brandName;

private:
    struct Weight {
        static constexpr int UNLIKELY = 1;
        static constexpr int NEUTRAL = 2;
        static constexpr int LIKELY = 10;
    };

private:
    /// Roll mod.
    uint32_t totalWeight;
    /**
     * Weight to ability ID map.
     * Calculated once upon class construction.
     */
    std::vector<std::string_view> cachedWeightsMap;

#pragma mark Seed
public:
    static uint32_t advanceSeed(uint32_t seed);

#pragma mark Roll abilities
public:
    std::string_view getBrandedAbility(uint32_t seed, std::string_view brandName);
    inline std::pair<std::string_view, uint32_t> generateRoll(uint32_t seed, std::string_view brandName);

#pragma mark Find seed
public:
    std::vector<uint32_t> findSeed(const std::vector<std::string_view>& previousRolls);
};


#endif //SPLATOON_3_GEAR_HELPER_CPP_SEED_HELPER_H