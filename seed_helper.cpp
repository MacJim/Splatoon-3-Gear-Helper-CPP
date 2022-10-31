//
// Created by Jim Shi on 2022-10-28.
//

#include "seed_helper.h"

#include <numeric>

#include "data/abilities.h"
#include "data/brands.h"


struct Weight {
    static constexpr int UNLIKELY = 1;
    static constexpr int NEUTRAL = 2;
    static constexpr int LIKELY = 10;
};


SeedHelper::SeedHelper(std::string_view brandName) {
    this->brandName = brandName;

    // Cache weights.
    /**
     * Each ability's weight.
     * Indices correspond to `abilities`.
     */
    std::array<uint32_t, abilities.size()> weights {};
    if (std::find(neutralBrands.begin(), neutralBrands.end(), brandName) != neutralBrands.end()) {
        // Neutral brand.
        std::fill(weights.begin(), weights.end(), 2);
    } else {
        // Biased brand.
        for (const auto& brand : biasedBrands) {
            if (std::get<0>(brand) != brandName) {
                continue;
            }

            // Found the ability.
            const std::string_view& likelyAbility = std::get<1>(brand);
            const std::string_view& unlikelyAbility = std::get<2>(brand);

            for (size_t i = 0; i < abilities.size(); i += 1) {
                if (abilities[i] == likelyAbility) {
                    weights[i] = Weight::LIKELY;
                } else if (abilities[i] == unlikelyAbility) {
                    weights[i] = Weight::UNLIKELY;
                } else {
                    weights[i] = Weight::NEUTRAL;
                }
            }

            break;
        }
    }

    this->totalWeight = std::accumulate(weights.begin(), weights.end(), 0);

    this->cachedWeightsMap = std::vector<std::string_view>();
    this->cachedWeightsMap.reserve(totalWeight);

    size_t currentIndex = 0;
    for (size_t i = 0; i < abilities.size(); i += 1) {
        size_t nextIndex = currentIndex + weights[i];
        for (size_t j = currentIndex; j < nextIndex; j += 1) {
            this->cachedWeightsMap.push_back(abilities[i]);
        }

        currentIndex = nextIndex;
    }
}

uint32_t SeedHelper::advanceSeed(uint32_t seed) {
    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);
    return seed;
}

std::string_view SeedHelper::getBrandedAbility(uint32_t seed) {
    assert(!brandName.empty());
    assert(totalWeight > 0);
    assert(!cachedWeightsMap.empty());

    const auto randomNumber = seed % totalWeight;
    return cachedWeightsMap[randomNumber];
}

std::pair<uint32_t, std::string_view> SeedHelper::generateRoll(uint32_t seed) {
    seed = advanceSeed(seed);
    const auto ability = getBrandedAbility(seed);
    return std::make_pair(seed, ability);
}

std::vector<uint32_t> SeedHelper::findSeed(const std::vector<std::string_view> &previousRolls) {
    auto returnValue = std::vector<uint32_t>();

    uint32_t initial_seed = 0;
    while (initial_seed++ != UINT32_MAX) {  // I hate `++`, but for an unsigned int this seems to be the best solution.
        auto seed = initial_seed;
        auto valid = true;
        for (size_t i = 0; i < previousRolls.size(); i += 1) {
            const auto result = generateRoll(seed);
            seed = result.first;
            if ((result.second != previousRolls[i]) && (result.second != placeholderAbility)) {
                valid = false;
                break;
            }
        }

        if (valid) {
            returnValue.push_back(initial_seed);
        }
    }

    return returnValue;
}


