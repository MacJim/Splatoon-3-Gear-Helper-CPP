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


SeedHelper::SeedHelper(std::string_view brandName): brandName(brandName), cachedWeights{}, cachedDrinkWeightsMap{} {
    // Calculate weight for each ability.
    if (std::find(neutralBrands.begin(), neutralBrands.end(), brandName) != neutralBrands.end()) {
        // Neutral brand.
        std::fill(cachedWeights.begin(), cachedWeights.end(), Weight::NEUTRAL);
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
                    cachedWeights[i] = Weight::LIKELY;
                } else if (abilities[i] == unlikelyAbility) {
                    cachedWeights[i] = Weight::UNLIKELY;
                } else {
                    cachedWeights[i] = Weight::NEUTRAL;
                }
            }

            break;
        }
    }

    this->totalWeight = std::accumulate(cachedWeights.begin(), cachedWeights.end(), 0);

    this->cachedWeightsMap = std::vector<std::string_view>();
    this->cachedWeightsMap.reserve(totalWeight);

    // Convert weights for each ability to a map for faster indexing.
    size_t currentIndex = 0;
    for (size_t i = 0; i < abilities.size(); i += 1) {
        size_t nextIndex = currentIndex + cachedWeights[i];
        for (size_t j = currentIndex; j < nextIndex; j += 1) {
            this->cachedWeightsMap.push_back(abilities[i]);
        }

        currentIndex = nextIndex;
    }
}

void SeedHelper::cacheDrinkWeightMap(const std::string_view &drink) {
    size_t drinkIndex = getAbilityIndex(drink);

    const size_t currentDrinkTotalWeight = totalWeight - cachedWeights[drinkIndex];

    std::vector<std::string_view> currentDrinkWeightsMap{};
    currentDrinkWeightsMap.reserve(currentDrinkTotalWeight);
    size_t currentIndex = 0;
    for (size_t i = 0; i < abilities.size(); i += 1) {
        if (i == drinkIndex) {
            // Drink ability has 0 weight.
            continue;
        }

        size_t nextIndex = currentIndex + cachedWeights[i];
        for (size_t j = currentIndex; j < nextIndex; j += 1) {
            currentDrinkWeightsMap.push_back(abilities[i]);
        }

        currentIndex = nextIndex;
    }

    cachedDrinkWeightsMap[drinkIndex] = std::make_pair(currentDrinkTotalWeight, currentDrinkWeightsMap);
}

uint32_t SeedHelper::advanceSeed(uint32_t seed) {
    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);
    return seed;
}

std::string_view SeedHelper::getBrandedAbility(uint32_t seed) const {
    assert(!brandName.empty());
    assert(totalWeight > 0);
    assert(!cachedWeightsMap.empty());

    const auto randomNumber = seed % totalWeight;
    return cachedWeightsMap[randomNumber];
}

std::string_view SeedHelper::getBrandedAbilityWithDrink(uint32_t seed, const std::string_view& drink) const {
    const auto drinkIndex = getAbilityIndex(drink);
    const auto& [currentDrinkTotalWeight, currentDrinkWeightsMap] = cachedDrinkWeightsMap[drinkIndex];

    const auto randomNumber = seed % currentDrinkTotalWeight;
    return currentDrinkWeightsMap[randomNumber];
}

std::pair<uint32_t, std::string_view> SeedHelper::generateRoll(uint32_t seed) const {
    seed = advanceSeed(seed);
    const auto ability = getBrandedAbility(seed);
    return std::make_pair(seed, ability);
}

std::pair<uint32_t, std::string_view> SeedHelper::generateRollWithDrink(uint32_t seed, const std::string_view& drink) const {
    seed = advanceSeed(seed);
    if (seed % 100 <= 29) {
        return std::make_pair(seed, drink);
    }

    seed = advanceSeed(seed);
    const auto ability = getBrandedAbilityWithDrink(seed, drink);
    return std::make_pair(seed, ability);
}

std::vector<uint32_t> SeedHelper::findSeed(const RollSequence &previousRolls) {
    // Cache weights with drinks applied.
    const auto drinksUsed = previousRolls.getDrinksUsed();
    for (const auto& drink: drinksUsed) {
        cacheDrinkWeightMap(drink);
    }

    // Brute force solution: Try all possible start seeds.
    auto returnValue = std::vector<uint32_t>();

    uint32_t initial_seed = 0;
    while (initial_seed++ != UINT32_MAX) {  // I hate `++`, but for an unsigned int this seems to be the best solution.
        auto seed = initial_seed;
        auto valid = true;
        for (const auto& [expectedResult, drink]: previousRolls) {
            std::string_view result;
            if (drink.empty()) {
                std::tie(seed, result) = generateRoll(seed);
            } else {
                std::tie(seed, result) = generateRollWithDrink(seed, drink);
            }

            if ((result != expectedResult) && (result != placeholderAbility)) {
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


