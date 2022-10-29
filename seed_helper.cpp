//
// Created by Jim Shi on 2022-10-28.
//

#include "seed_helper.h"

#include <numeric>

#include "data/abilities.h"
#include "data/brands.h"


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
    return 0;
}


