//
// Created by Jim Shi on 2022-10-28.
//

#include "seed_helper.h"

#include <numeric>
#include <future>

#include "data/brand.h"


struct Weight {
    static constexpr int UNLIKELY = 1;
    static constexpr int NEUTRAL = 2;
    static constexpr int LIKELY = 10;
};


SeedHelper::SeedHelper(std::string_view brandName): brandName(brandName), cachedWeights{}, rollToAbilityMap{},
                                                    drinkRollToAbilityMap{} {
    using AbilityHelper::abilitiesCount;

    // Calculate weight for each ability.
    if (std::find(neutralBrands.begin(), neutralBrands.end(), brandName) != neutralBrands.end()) {
        // Neutral brand.
        std::fill(cachedWeights.begin(), cachedWeights.end(), Weight::NEUTRAL);
    } else {
        // Biased brand.
        for (const auto [brandName2, likelyAbility, unlikelyAbility]: biasedBrands) {
            if (brandName2 == brandName) {
                // Found the brand.
                for (size_t i = 0; i < abilitiesCount; i += 1) {
                    const auto ability = static_cast<Ability>(i);
                    if (ability == likelyAbility) {
                        cachedWeights[i] = Weight::LIKELY;
                    } else if (ability == unlikelyAbility) {
                        cachedWeights[i] = Weight::UNLIKELY;
                    } else {
                        cachedWeights[i] = Weight::NEUTRAL;
                    }
                }

                break;
            }
        }
    }

    this->totalWeight = std::accumulate(cachedWeights.begin(), cachedWeights.end(), 0);

    // Convert weights for each ability to a map for faster indexing.
    this->rollToAbilityMap.reserve(totalWeight);

    size_t currentIndex = 0;
    for (size_t i = 0; i < abilitiesCount; i += 1) {
        const auto ability = static_cast<Ability>(i);

        size_t nextIndex = currentIndex + cachedWeights[i];
        for (size_t j = currentIndex; j < nextIndex; j += 1) {
            this->rollToAbilityMap.push_back(ability);
        }

        currentIndex = nextIndex;
    }
}

void SeedHelper::cacheDrinkRollToAbilityMap(const Ability drink) {
    size_t drinkIndex = AbilityHelper::getIndex(drink);

    const size_t currentDrinkTotalWeight = totalWeight - cachedWeights[drinkIndex];

    std::vector<Ability> currentDrinkWeightsMap{};
    currentDrinkWeightsMap.reserve(currentDrinkTotalWeight);
    size_t currentIndex = 0;
    for (size_t i = 0; i < AbilityHelper::abilitiesCount; i += 1) {
        if (i == drinkIndex) {
            // Drink ability has 0 weight.
            continue;
        }

        size_t nextIndex = currentIndex + cachedWeights[i];
        for (size_t j = currentIndex; j < nextIndex; j += 1) {
            currentDrinkWeightsMap.push_back(static_cast<Ability>(i));
        }

        currentIndex = nextIndex;
    }

    drinkRollToAbilityMap[drinkIndex] = std::make_pair(currentDrinkTotalWeight, currentDrinkWeightsMap);
}

void SeedHelper::cacheAllDrinkRollToAbilityMaps() {
    for (size_t i = 0; i < AbilityHelper::abilitiesCount; i += 1) {
        const auto drink = static_cast<Ability>(i);
        cacheDrinkRollToAbilityMap(drink);
    }
}

uint32_t SeedHelper::advanceSeed(uint32_t seed) {
    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);
    return seed;
}

std::pair<bool, uint32_t> SeedHelper::advanceSeedToEndOfRollSequence(const uint32_t initialSeed, const RollSequence &rollSequence) {
    // Cache weights with drinks applied.
    const auto drinksUsed = rollSequence.getDrinksUsed();
    for (const auto drink: drinksUsed) {
        cacheDrinkRollToAbilityMap(drink);
    }

    bool validity = true;
    uint32_t seed = initialSeed;

    for (const auto [expectedAbility, drink]: rollSequence) {
        Ability ability;
        if (drink == Ability::noDrink) {
            std::tie(seed, ability) = generateRoll(seed);
        } else {
            std::tie(seed, ability) = generateRollWithDrink(seed, drink);
        }

        if (ability != expectedAbility) {
            validity = false;
        }
    }

    return std::make_pair(validity, seed);
}

Ability SeedHelper::getBrandedAbility(uint32_t seed) const {
    assert(!brandName.empty());
    assert(totalWeight > 0);
    assert(!rollToAbilityMap.empty());

    const auto roll = seed % totalWeight;
    return rollToAbilityMap[roll];
}

Ability SeedHelper::getBrandedAbilityWithDrink(uint32_t seed, const Ability drink) const {
    assert(drink != Ability::noDrink);

    const auto drinkIndex = AbilityHelper::getIndex(drink);
    const auto& [currentDrinkTotalWeight, currentDrinkWeightsMap] = drinkRollToAbilityMap[drinkIndex];
    assert(currentDrinkTotalWeight > 0);  // Assert that `cacheDrinkRollToAbilityMap` has been called for the current drink.

    const auto roll = seed % currentDrinkTotalWeight;
    return currentDrinkWeightsMap[roll];
}

std::pair<uint32_t, Ability> SeedHelper::generateRoll(uint32_t seed) const {
    seed = advanceSeed(seed);
    const auto ability = getBrandedAbility(seed);
    return std::make_pair(seed, ability);
}

std::pair<uint32_t, Ability> SeedHelper::generateRollWithDrink(uint32_t seed, const Ability drink) const {
    seed = advanceSeed(seed);
    const auto roll = seed % 100;
    if (roll <= 29) {
        return std::make_pair(seed, drink);
    }

    seed = advanceSeed(seed);
    const auto ability = getBrandedAbilityWithDrink(seed, drink);
    return std::make_pair(seed, ability);
}

std::vector<Ability> SeedHelper::generateRolls(uint32_t seed, const size_t length) const {
    std::vector<Ability> returnValue{length, Ability::unknown};
    for (size_t i = 0; i < length; i += 1) {
        std::tie(seed, returnValue[i]) = generateRoll(seed);
    }

    return returnValue;
}

std::vector<Ability> SeedHelper::generateRollsWithDrink(uint32_t seed, Ability drink, size_t length) {
    cacheDrinkRollToAbilityMap(drink);

    std::vector<Ability> returnValue{length, Ability::unknown};
    for (size_t i = 0; i < length; i += 1) {
        std::tie(seed, returnValue[i]) = generateRollWithDrink(seed, drink);
    }

    return returnValue;
}

std::vector<uint32_t> SeedHelper::findSeedWorker(const RollSequence &previousRolls, const uint32_t seedStart, const uint32_t seedStop) const {
    assert(seedStart <= seedStop);

    // Brute force solution: Try all possible start seeds.
    auto returnValue = std::vector<uint32_t>();

    uint32_t initial_seed = seedStart;
    do {
        auto seed = initial_seed;
        auto valid = true;
        for (const auto [expectedResult, drink]: previousRolls) {
            Ability result;
            if (drink == Ability::noDrink) {
                std::tie(seed, result) = generateRoll(seed);
            } else {
                std::tie(seed, result) = generateRollWithDrink(seed, drink);
            }

            if ((result != expectedResult) && (result != Ability::unknown)) {
                valid = false;
                break;
            }
        }

        if (valid) {
            returnValue.push_back(initial_seed);
        }
    } while (initial_seed++ != seedStop);  // I hate `++`, but for an unsigned int this seems to be the best solution.

    return returnValue;
}

std::vector<uint32_t> SeedHelper::findSeed(const RollSequence &previousRolls, const size_t workersCount) {
    // Cache weights with drinks applied.
    const auto drinksUsed = previousRolls.getDrinksUsed();
    for (const auto drink: drinksUsed) {
        cacheDrinkRollToAbilityMap(drink);
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
