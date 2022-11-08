//
// Created by Jim Shi on 2022-10-28.
//

#include "seed_helper.h"

#include <numeric>
#include <future>

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
            const std::string_view likelyAbility = std::get<1>(brand);
            const std::string_view unlikelyAbility = std::get<2>(brand);

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

void SeedHelper::cacheDrinkWeightsMap(const std::string_view drink) {
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

void SeedHelper::cacheAllDrinkWeightsMaps() {
    for (const auto& drink: abilities) {
        cacheDrinkWeightsMap(drink);
    }
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

std::string_view SeedHelper::getBrandedAbilityWithDrink(uint32_t seed, const std::string_view drink) const {
    assert(!drink.empty());

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

std::pair<uint32_t, std::string_view> SeedHelper::generateRollWithDrink(uint32_t seed, const std::string_view drink) const {
    seed = advanceSeed(seed);
    const auto randomNumber = seed % 100;
    if (randomNumber <= 29) {
        return std::make_pair(seed, drink);
    }

    seed = advanceSeed(seed);
    const auto ability = getBrandedAbilityWithDrink(seed, drink);
    return std::make_pair(seed, ability);
}

std::vector<uint32_t> SeedHelper::findSeedWorker(const RollSequence &previousRolls, const uint32_t seedStart, const uint32_t seedStop) const {
    // Brute force solution: Try all possible start seeds.
    auto returnValue = std::vector<uint32_t>();

    uint32_t initial_seed = seedStart;
    while (initial_seed++ != seedStop) {  // I hate `++`, but for an unsigned int this seems to be the best solution.
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

std::vector<uint32_t> SeedHelper::findSeed(const RollSequence &previousRolls, const size_t workersCount) {
    // Cache weights with drinks applied.
    const auto drinksUsed = previousRolls.getDrinksUsed();
    for (const auto& drink: drinksUsed) {
        cacheDrinkWeightsMap(drink);
    }

    if (workersCount == 0) {
        // Don't use worker threads.
        return findSeedWorker(previousRolls, 0, UINT32_MAX);
    } else {
        // Use multiple threads.
        std::vector<std::future<std::vector<uint32_t>>> futures{};
        futures.reserve(workersCount);

        // First (workersCount - 1) tasks.
        for (size_t i = 0; i < (workersCount - 1); i += 1) {
            const auto seedStart = UINT32_MAX / workersCount * i;
            const auto seedStop = UINT32_MAX / workersCount * (i + 1) - 1;
            auto currentFuture = std::async(std::launch::async, [this, &previousRolls, seedStart, seedStop]() {
                return this->findSeedWorker(previousRolls, seedStart, seedStop);
            });  // Dirty workaround using lambda 😂.
            futures.push_back(std::move(currentFuture));
        }
        // Final task: Note `finalStop`.
        auto finalFuture = std::async(std::launch::async, [this, &previousRolls, &workersCount]() {
            const auto finalStart = UINT32_MAX / workersCount * (workersCount - 1);
            constexpr auto finalStop = UINT32_MAX;
            return this->findSeedWorker(previousRolls, finalStart, finalStop);
        });
        futures.push_back(std::move(finalFuture));

        // Construct return value.
        std::vector<uint32_t> returnValue{};
        for (auto& future: futures) {
            const auto workerResults = future.get();
            returnValue.insert(returnValue.end(), workerResults.begin(), workerResults.end());
        }

        return returnValue;
    }
}


