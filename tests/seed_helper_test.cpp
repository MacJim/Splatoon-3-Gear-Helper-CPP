#include <array>
#include <unordered_map>
#include <set>

#include "gtest/gtest.h"

#include "../seed_helper.h"
#include "../data/brand.h"


#pragma mark advanceSeed
TEST(SeedHelperTest, AdvanceSeed) {
    constexpr std::array<std::array<uint32_t, 7>, 5> testCases = {
        0x1, 0x42021, 0x4080601, 0x9dcca8c5, 0x1255994f, 0x8ef917d1, 0x2c6f5bd0,
        0x2, 0x84042, 0x8008c02, 0x2998d882, 0x6398b906, 0xe3b0281a, 0x30d07323,
        0x3, 0xc6063, 0xc088a03, 0xb4547047, 0x71cd2049, 0x6d493fcb, 0x1cbf28f3,
        0x4, 0x108084, 0x10011804, 0x5331b104, 0xc731720c, 0xc7605034, 0x61b06667,
        0xabcdef, 0x5bd43de4, 0x5172a9d0, 0x8d5dd574, 0xc99a9f2d, 0xd5913052, 0x80fa3a7f,
    };

    for (auto& testCase: testCases) {
        uint32_t calculatedSeed = testCase[0];
        for (uint32_t seed: testCase) {
            EXPECT_EQ(calculatedSeed, seed) << "Test case: " << ::testing::PrintToString(testCase);
            calculatedSeed = SeedHelper::advanceSeed(calculatedSeed);
        }
    }
}


TEST(SeedHelperTest, AdvanceSeedToEndOfRollSequenceEmptyRoll) {
    // Test case.
    const std::string brand{"Tentatek"};
    constexpr uint32_t initialSeed = 0x12345678;
    const RollSequence rollSequence{};
    constexpr uint32_t expectedFinalSeed = initialSeed;

    // Test.
    SeedHelper seedHelper{brand};
    const auto [validity, finalSeed] = seedHelper.advanceSeedToEndOfRollSequence(initialSeed, rollSequence);
    EXPECT_TRUE(validity);
    EXPECT_EQ(finalSeed, expectedFinalSeed);
}


TEST(SeedHelperTest, AdvanceSeedToEndOfRollSequenceNoDrink) {
    // Test case.
    const std::string brand{"Toni Kensa"};
    constexpr uint32_t initialSeed = 0xb0980324;
    const std::vector<Ability> rolledAbilities {
        Ability::inkSaverMain,
        Ability::specialPowerUp,
        Ability::inkSaverMain,
        Ability::inkSaverMain,
        Ability::inkSaverMain,
        Ability::specialPowerUp,
        Ability::specialSaver,
        Ability::intensifyAction,
        Ability::subResistanceUp,
        Ability::intensifyAction,
    };
    const RollSequence rollSequence{rolledAbilities};
    constexpr uint32_t expectedFinalSeed = 0x88554788;

    // Test.
    SeedHelper seedHelper{brand};
    const auto [validity, finalSeed] = seedHelper.advanceSeedToEndOfRollSequence(initialSeed, rollSequence);
    EXPECT_TRUE(validity);
    EXPECT_EQ(finalSeed, expectedFinalSeed);
}


TEST(SeedHelperTest, AdvanceSeedToEndOfRollSequenceWithDrink) {
    // Test case.
    const std::string brand{"Zink"};
    constexpr uint32_t initialSeed = 0x907b1ae9;
    constexpr Ability drink = Ability::quickSuperJump;
    const std::vector<Ability> rolledAbilities {
        Ability::quickSuperJump,
        Ability::quickSuperJump,
        Ability::quickSuperJump,
        Ability::specialPowerUp,
        Ability::swimSpeedUp,
        Ability::specialSaver,
        Ability::quickSuperJump,
        Ability::inkSaverMain,
        Ability::quickSuperJump,
        Ability::inkRecoveryUp,
    };
    RollSequence rollSequence{};
    for (const auto ability: rolledAbilities) {
        rollSequence.addRoll(ability, drink);
    }
    constexpr uint32_t expectedFinalSeed = 0x6aeddb71;

    // Test.
    SeedHelper seedHelper{brand};
    const auto [validity, finalSeed] = seedHelper.advanceSeedToEndOfRollSequence(initialSeed, rollSequence);
    EXPECT_TRUE(validity);
    EXPECT_EQ(finalSeed, expectedFinalSeed);
}


#pragma mark getBrandedAbility
TEST(SeedHelperTest, GetBrandedAbilityNeutralBrands) {
    /// (seed, expected result)
    constexpr std::array<std::pair<uint32_t, Ability>, 22> testCases = {
        std::make_pair(0x0, Ability::inkSaverMain),
        std::make_pair(0x42021, Ability::inkSaverMain),
        std::make_pair(0x84042, Ability::inkSaverSub),
        std::make_pair(0xc6063, Ability::inkSaverSub),
        std::make_pair(0x108084, Ability::inkRecoveryUp),
        std::make_pair(0x14a0a5, Ability::inkRecoveryUp),
        std::make_pair(0x18c0c6, Ability::runSpeedUp),
        std::make_pair(0x1ce0e7, Ability::runSpeedUp),

        // Obtained from the seed sequence starting with 0x1.
        std::make_pair(0x60b66de0, Ability::inkSaverMain),
        std::make_pair(0xcf2eecfa, Ability::inkSaverSub),
        std::make_pair(0xb945bae9, Ability::inkRecoveryUp),
        std::make_pair(0x4eb10b7, Ability::runSpeedUp),
        std::make_pair(0x13223391, Ability::swimSpeedUp),
        std::make_pair(0x52f4d3eb, Ability::specialChargeUp),
        std::make_pair(0x9f2de485, Ability::specialSaver),
        std::make_pair(0xa956130a, Ability::specialPowerUp),
        std::make_pair(0xdcc65bd4, Ability::quickRespawn),
        std::make_pair(0x379686c7, Ability::quickSuperJump),
        std::make_pair(0xc651068d, Ability::subPowerUp),
        std::make_pair(0x19f91cb2, Ability::inkResistanceUp),
        std::make_pair(0xddbb4b1, Ability::subResistanceUp),
        std::make_pair(0x591c9737, Ability::intensifyAction),
    };

    for (auto& brandName: neutralBrands) {
        auto seedHelper = SeedHelper(brandName);
        for (auto& [seed, expectedAbility]: testCases) {
            const auto ability = seedHelper.getBrandedAbility(seed);
            EXPECT_EQ(ability, expectedAbility) << "Test case: (" << brandName << ", 0x" << std::hex << seed << ")";
        }
    }
}


TEST(SeedHelperTest, GetBrandedAbilityBiasedBrands) {
    /**
     * {brand name: (seed, expected result)}
     * Order: Likely, unlikely, neutral
     */
    const std::unordered_map<std::string_view, std::array<std::pair<uint32_t, Ability>, 3>> testCases = {
        {"Annaki", {
            std::make_pair(0x887e9a54, Ability::inkSaverSub),
            std::make_pair(0x36cc0f25, Ability::specialSaver),
            std::make_pair(0x436dfc0, Ability::inkRecoveryUp),
        }},
        {"Barazushi", {
           std::make_pair(0x8b3644cf, Ability::intensifyAction),
           std::make_pair(0x4eab4f2f, Ability::subPowerUp),
           std::make_pair(0x39d2b193, Ability::quickRespawn),
        }},
        {"Emberz", {
           std::make_pair(0x4111a2fd, Ability::intensifyAction),
           std::make_pair(0x94edd48f, Ability::specialChargeUp),
           std::make_pair(0xff675892, Ability::runSpeedUp),
        }},
        {"Enperry", {
           std::make_pair(0xe90c15d3, Ability::subPowerUp),
           std::make_pair(0xf6bd5e1f, Ability::inkResistanceUp),
           std::make_pair(0x5a77d10, Ability::swimSpeedUp),
        }},
    };

    for (auto& [brandName, brandTestCases]: testCases) {
        auto seedHelper = SeedHelper(brandName);
        for (auto& [seed, expectedAbility]: brandTestCases) {
            const auto ability = seedHelper.getBrandedAbility(seed);
            EXPECT_EQ(ability, expectedAbility) << "Test case: (" << brandName << ", 0x" << std::hex << seed << ")";
        }
    }
}


#pragma mark generateRoll and generateRolls
TEST(SeedHelperTest, GenerateRollNeutralBrands) {
    constexpr uint32_t initialSeed = 0x81207561;
    constexpr std::array<std::pair<uint32_t, Ability>, 7> expectedResults = {
        std::make_pair(0x7e0e4647, Ability::specialPowerUp),
        std::make_pair(0x6e1959a4, Ability::swimSpeedUp),
        std::make_pair(0xe0929d72, Ability::intensifyAction),
        std::make_pair(0xd4ac196c, Ability::swimSpeedUp),
        std::make_pair(0xa7b7e72c, Ability::subResistanceUp),
        std::make_pair(0x311b1a25, Ability::specialSaver),
        std::make_pair(0x19adf24a, Ability::runSpeedUp),
    };

    // generateRoll
    for (auto& brandName: neutralBrands) {
        auto seedHelper = SeedHelper(brandName);

        uint32_t seed = initialSeed;
        for (auto [expectedSeed, expectedAbility]: expectedResults) {
            const auto& result = seedHelper.generateRoll(seed);
            seed = result.first;
            EXPECT_EQ(seed, expectedSeed) << "Test case: (" << brandName << ", 0x" << std::hex << initialSeed << ")";
            EXPECT_EQ(result.second, expectedAbility) << "Test case: (" << brandName << ", 0x" << std::hex << initialSeed << ")";
        }
    }

    // generateRolls
    for (auto& brandName: neutralBrands) {
        auto seedHelper = SeedHelper(brandName);

        const auto results = seedHelper.generateRolls(initialSeed, expectedResults.size());
        EXPECT_EQ(results.size(), expectedResults.size());
        for (size_t i = 0; i < results.size(); i += 1) {
            EXPECT_EQ(results[i], expectedResults[i].second) << "Index: " << i;
        }
    }
}


/// `generateRoll` only.
TEST(SeedHelperTest, GenerateRollBiasedBrands) {
    /**
     * {brand name: (seed, expected next seed, expected result)}
     * Order: Likely, unlikely, neutral
     */
    const std::unordered_map<std::string_view, std::array<std::tuple<uint32_t, uint32_t, Ability>, 3>> testCases = {
        {"Krak-On", {
            std::make_tuple(0xa5b4a2ae, 0xdce319e, Ability::swimSpeedUp),
            std::make_tuple(0x14415c0a, 0xc7c07eaa, Ability::subResistanceUp),
            std::make_tuple(0x581853e9, 0x1ece01bb, Ability::quickRespawn),
        }},
        {"Splash Mob", {
            std::make_tuple(0x10b8c0fe, 0x1c47910d, Ability::inkSaverMain),
            std::make_tuple(0x236779ee, 0x470127a3, Ability::runSpeedUp),
            std::make_tuple(0x850f3db7, 0x3584718b, Ability::inkRecoveryUp),
        }},
    };

    for (auto& [brandName, brandTestCases]: testCases) {
        auto seedHelper = SeedHelper(brandName);
        for (auto& [seed, expectedNextSeed, expectedAbility]: brandTestCases) {
            const auto& result = seedHelper.generateRoll(seed);
            EXPECT_EQ(result.first, expectedNextSeed) << "Test case: (" << brandName << ", 0x" << std::hex << seed << ")";
            EXPECT_EQ(result.second, expectedAbility) << "Test case: (" << brandName << ", 0x" << std::hex << seed << ")";
        }
    }
}


/// `generateRolls` only.
TEST(SeedHelperTest, GenerateRollsBiasedBrands) {
    constexpr uint32_t initialSeed = 0x23147098;
    constexpr std::string_view brandName = "Splash Mob";
    const std::vector<Ability> testCases {
        Ability::specialPowerUp,
        Ability::inkSaverMain,
        Ability::specialSaver,
        Ability::runSpeedUp,
        Ability::quickRespawn,
        Ability::specialChargeUp,
        Ability::specialPowerUp,
        Ability::inkResistanceUp,
        Ability::quickSuperJump,
        Ability::inkSaverMain,
        Ability::runSpeedUp,
        Ability::specialPowerUp,
        Ability::swimSpeedUp,
        Ability::swimSpeedUp,
        Ability::inkSaverMain,
    };

    SeedHelper seedHelper{brandName};
    const auto results = seedHelper.generateRolls(initialSeed, testCases.size());
    EXPECT_EQ(results, testCases);
}


#pragma mark generateRollWithDrink and generateRollsWithDrink
TEST(SeedHelperTest, GenerateRollWithDrinkNeutralBrands) {
    constexpr uint32_t initialSeed = 0x79239fed;
    constexpr auto drink = Ability::runSpeedUp;
    /// (expected seed, expected result)
    constexpr std::array<std::pair<uint32_t, Ability>, 10> expectedResults = {
        std::make_pair(0x51196ac2, Ability::runSpeedUp),
        std::make_pair(0x2b86c939, Ability::subResistanceUp),
        std::make_pair(0xa6939d49, Ability::runSpeedUp),
        std::make_pair(0x8e984091, Ability::subPowerUp),
        std::make_pair(0x7673e6, Ability::inkResistanceUp),
        std::make_pair(0xce455fc3, Ability::swimSpeedUp),
        std::make_pair(0xd21caebd, Ability::runSpeedUp),
        std::make_pair(0xa2d39c2a, Ability::subPowerUp),
        std::make_pair(0xfb8024a1, Ability::runSpeedUp),
        std::make_pair(0xb41a6d22, Ability::specialPowerUp),
    };

    // generateRollWithDrink
    for (auto& brandName: neutralBrands) {
        auto seedHelper = SeedHelper(brandName);
        seedHelper.cacheAllDrinkRollToAbilityMaps();

        uint32_t seed = initialSeed;
        for (auto& [expectedSeed, expectedAbility]: expectedResults) {
            const auto& result = seedHelper.generateRollWithDrink(seed, drink);
            seed = result.first;
            EXPECT_EQ(seed, expectedSeed) << "Test case: (" << brandName << ", 0x" << std::hex << initialSeed << ")";
            EXPECT_EQ(result.second, expectedAbility) << "Test case: (" << brandName << ", 0x" << std::hex << initialSeed << ")";
        }
    }

    // generateRollsWithDrink
    for (auto& brandName: neutralBrands) {
        auto seedHelper = SeedHelper(brandName);

        const auto results = seedHelper.generateRollsWithDrink(initialSeed, drink, expectedResults.size());
        EXPECT_EQ(results.size(), expectedResults.size());
        for (size_t i = 0; i < results.size(); i += 1) {
            EXPECT_EQ(results[i], expectedResults[i].second) << "Index: " << i;
        }
    }
}


TEST(SeedHelperTest, GenerateRollWithDrinkBiasedBrands) {
    using ExpectedSequence = std::vector<std::pair<uint32_t, Ability>>;
    /// (brand name, initial seed, drink, (expected seed, expected ability))
    const std::vector<std::tuple<std::string_view, uint32_t, Ability, ExpectedSequence>> testCases {
        std::make_tuple("Forge", 0x17239048, Ability::quickRespawn, ExpectedSequence{
            std::make_pair(0xf57b8c75, Ability::intensifyAction),
            std::make_pair(0x1a58afef, Ability::quickRespawn),
            std::make_pair(0xfb0c4f9d, Ability::quickRespawn),
            std::make_pair(0x2d050aa2, Ability::quickRespawn),
            std::make_pair(0x6709dca, Ability::quickRespawn),
            std::make_pair(0x9f7c6099, Ability::intensifyAction),
            std::make_pair(0x7e866cee, Ability::quickRespawn),
            std::make_pair(0x13e59081, Ability::specialSaver),
            std::make_pair(0xf7f1178e, Ability::subResistanceUp),
            std::make_pair(0x475c66a9, Ability::swimSpeedUp),
        }),
        std::make_tuple("Inkline", 0x231897b1, Ability::inkSaverSub, ExpectedSequence{
            std::make_pair(0xc3b4786, Ability::inkSaverSub),
            std::make_pair(0xcd25bf65, Ability::subResistanceUp),
            std::make_pair(0x23ed6221, Ability::inkSaverSub),
            std::make_pair(0xf1848924, Ability::subResistanceUp),
            std::make_pair(0x74a717f4, Ability::inkSaverSub),
            std::make_pair(0x5d6247d8, Ability::inkSaverSub),
            std::make_pair(0xa6b0ef94, Ability::inkSaverSub),
            std::make_pair(0xef49b19e, Ability::specialSaver),
            std::make_pair(0xf639b943, Ability::inkSaverSub),
            std::make_pair(0x554cdf72, Ability::inkSaverMain),
        }),
        std::make_tuple("Rockenberg", 0xa8b97928, Ability::subPowerUp, ExpectedSequence{
            std::make_pair(0x32ae369, Ability::inkSaverSub),
            std::make_pair(0xb7ba758a, Ability::subPowerUp),
            std::make_pair(0x3a878850, Ability::runSpeedUp),
            std::make_pair(0xba305f56, Ability::subPowerUp),
            std::make_pair(0x5fbfe477, Ability::inkResistanceUp),
            std::make_pair(0xbb50d442, Ability::runSpeedUp),
            std::make_pair(0x9ac0516e, Ability::subPowerUp),
            std::make_pair(0x10afb152, Ability::quickRespawn),
            std::make_pair(0x48c7666b, Ability::runSpeedUp),
            std::make_pair(0x2540d9ae, Ability::subPowerUp),
        }),
    };

    // generateRollWithDrink
    for (const auto& [brandName, initialSeed, drink, brandTestCases]: testCases) {
        auto seedHelper = SeedHelper(brandName);
        seedHelper.cacheDrinkRollToAbilityMap(drink);

        uint32_t seed = initialSeed;
        for (auto& [expectedNextSeed, expectedAbility]: brandTestCases) {
            const auto& result = seedHelper.generateRollWithDrink(seed, drink);
            seed = result.first;
            EXPECT_EQ(result.first, expectedNextSeed) << "Test case: (" << brandName << ", 0x" << std::hex << seed << ", 0x" << expectedNextSeed << ")";
            EXPECT_EQ(result.second, expectedAbility) << "Test case: (" << brandName << ", 0x" << std::hex << seed << ", 0x" << expectedNextSeed << ")";
        }
    }

    // generateRollsWithDrink
    for (const auto& [brandName, initialSeed, drink, brandTestCases]: testCases) {
        auto seedHelper = SeedHelper(brandName);

        const auto results = seedHelper.generateRollsWithDrink(initialSeed, drink, brandTestCases.size());
        EXPECT_EQ(results.size(), brandTestCases.size());
        for (size_t i = 0; i < results.size(); i += 1) {
            EXPECT_EQ(results[i], brandTestCases[i].second) << "Index: " << i;
        }
    }
}


#pragma mark findSeed, no drink
TEST(SeedHelperTest, FindSeedNeutralBrandsNoDrink) {
    /// (expected results/initial seed, rolled abilities)
    const std::vector<std::pair<std::vector<uint32_t>, std::vector<Ability>>> testCases = {
        std::make_pair(
            std::vector<uint32_t>({0x87a4b37e}),
            std::vector<Ability>({Ability::runSpeedUp, Ability::specialChargeUp, Ability::specialChargeUp, Ability::subPowerUp, Ability::runSpeedUp, Ability::specialSaver, Ability::specialChargeUp, Ability::intensifyAction, Ability::specialChargeUp, Ability::specialSaver, Ability::specialChargeUp, Ability::specialSaver, Ability::swimSpeedUp, Ability::specialSaver, Ability::specialSaver})
        ),
        std::make_pair(
            std::vector<uint32_t>({0x3b09c31b, 0xb32ef1ef, 0xd929930b, 0xea2db1fe}),
            std::vector<Ability>({Ability::inkSaverSub, Ability::inkResistanceUp, Ability::specialPowerUp, Ability::inkSaverSub, Ability::inkRecoveryUp, Ability::subPowerUp, Ability::runSpeedUp, Ability::specialPowerUp})
        ),
        std::make_pair(
            std::vector<uint32_t>({0x6190ed, 0x9778b27, 0xad66e54, 0xe4d3afa, 0x12e71833, 0x14054d6b, 0x17e85407, 0x1bbfa76e, 0x1c20f670, 0x22b76a45, 0x26ccf63f, 0x2931987c, 0x2ac8b6c2, 0x2b45e545, 0x2c3341f1, 0x2cbff1ff, 0x38bd30f2, 0x42e9f7fd, 0x498958ec, 0x51de83ff, 0x766cd6ba, 0x78cf83f3, 0x89ed9033, 0x8b687e8f, 0x8c465ea7, 0x8dba02ed, 0x91238fad, 0x94fe7fd0, 0xa268c374, 0xa3440b6b, 0xaa358c67, 0xad1ad783, 0xb21c2cff, 0xbc9ba1e5, 0xd4a5f808, 0xda744104, 0xe425c810, 0xeecadc56, 0xefabfaa1, 0xf452fc06, 0xf63dcb84}),
            std::vector<Ability>({Ability::inkSaverSub, Ability::quickRespawn, Ability::inkRecoveryUp, Ability::inkSaverMain, Ability::specialChargeUp, Ability::specialPowerUp, Ability::specialSaver})
        ),
    };

    for (auto& brandName: neutralBrands) {
        auto seedHelper = SeedHelper(brandName);
        for (auto& [expectedResults, rolledAbilities]: testCases) {
            const RollSequence rollSequence{rolledAbilities};
            const auto results = seedHelper.findSeed(rollSequence);
            EXPECT_EQ(results.size(), expectedResults.size()) << "Test case: (" << brandName << ", 0x" << std::hex << ::testing::PrintToString(expectedResults) << ")";

            const auto resultsSet = std::set<uint32_t>(results.begin(), results.end());
            const auto expectedResultsSet = std::set<uint32_t>(expectedResults.begin(), expectedResults.end());
            EXPECT_EQ(resultsSet, expectedResultsSet) << "Test case: (" << brandName << ", " << std::hex << ::testing::PrintToString(expectedResults) << ")";
        }
    }
}


TEST(SeedHelperTest, FindSeedBiasedBrandsNoDrink) {
    /// (brand name, expected results/initial seed, rolled abilities)
    const std::vector<std::tuple<std::string_view, std::vector<uint32_t>, std::vector<Ability>>> testCases = {
        std::make_tuple(
            "Zekko",
            std::vector<uint32_t>({0x87b091}),
            std::vector<Ability>({Ability::subResistanceUp, Ability::specialSaver, Ability::quickSuperJump, Ability::swimSpeedUp, Ability::specialPowerUp, Ability::quickRespawn, Ability::quickSuperJump, Ability::inkSaverMain, Ability::inkSaverSub, Ability::inkSaverSub})
        ),
        std::make_tuple(
            "Firefin",
            std::vector<uint32_t>({0x2ff2311e, 0x5cf03863, 0x809b1a97}),
            std::vector<Ability>({Ability::inkSaverSub, Ability::quickRespawn, Ability::subResistanceUp, Ability::swimSpeedUp, Ability::inkResistanceUp, Ability::quickRespawn, Ability::inkSaverMain, Ability::swimSpeedUp})
        ),
        std::make_tuple(
            "Splash Mob",
            std::vector<uint32_t>({0x7843744, 0x1b701a85, 0x21c98a86, 0x351c815a, 0x3810b1d3, 0x3cc89170, 0x58c56bc9, 0x91882513, 0x9ba02391, 0xa55de1c5, 0xbea8715e, 0xbf274b0d, 0xf6249c6c}),
            std::vector<Ability>({Ability::swimSpeedUp, Ability::inkSaverSub, Ability::quickSuperJump, Ability::inkSaverMain, Ability::inkSaverMain, Ability::subResistanceUp, Ability::inkSaverMain, Ability::subPowerUp, Ability::inkSaverMain})
        ),
    };

    for (auto& [brandName, expectedResults, rolledAbilities]: testCases) {
        const RollSequence rollSequence{rolledAbilities};
        auto seedHelper = SeedHelper(brandName);

        const auto results = seedHelper.findSeed(rollSequence);
        EXPECT_EQ(results.size(), expectedResults.size()) << "Test case: (" << brandName << ", " << std::hex << ::testing::PrintToString(expectedResults) << ")";

        const auto resultsSet = std::set<uint32_t>(results.begin(), results.end());
        const auto expectedResultsSet = std::set<uint32_t>(expectedResults.begin(), expectedResults.end());
        EXPECT_EQ(resultsSet, expectedResultsSet) << "Test case: (" << brandName << ", " << std::hex << ::testing::PrintToString(expectedResults) << ")";
    }
}


#pragma mark findSeed, with drink
TEST(SeedHelperTest, FindSeedNeutralBrandsWithDrink) {
    /// (expected results/initial seeds, (rolled ability, drink))
    const std::vector<std::pair<std::vector<uint32_t>, std::vector<std::pair<Ability, Ability>>>> testCases {
        std::make_pair(
            std::vector<uint32_t>{0xeef7090, 0x200012d3, 0x201b0697, 0x26f060cf, 0x4260d297, 0xf41a453d},  // After 2 rolls: {0xc3e69912, 0xf8a0a7b6, 0x268bf9a2, 0x6389287b, 0x949facc2, 0x4cbb7136}
            std::vector<std::pair<Ability, Ability>>{
                std::make_pair(Ability::inkResistanceUp, Ability::noDrink),
                std::make_pair(Ability::quickSuperJump, Ability::noDrink),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::subResistanceUp, Ability::quickRespawn),
                std::make_pair(Ability::specialPowerUp, Ability::quickRespawn),
                std::make_pair(Ability::inkResistanceUp, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::runSpeedUp, Ability::quickRespawn),
            }
        ),
        std::make_pair(
            std::vector<uint32_t>{0x9dbce285, 0xb06fb5d3, 0xfaac03bf},
            std::vector<std::pair<Ability, Ability>>{
                std::make_pair(Ability::intensifyAction, Ability::swimSpeedUp),
                std::make_pair(Ability::swimSpeedUp, Ability::swimSpeedUp),
                std::make_pair(Ability::swimSpeedUp, Ability::swimSpeedUp),
                std::make_pair(Ability::inkSaverMain, Ability::swimSpeedUp),
                std::make_pair(Ability::subResistanceUp, Ability::swimSpeedUp),
                std::make_pair(Ability::swimSpeedUp, Ability::swimSpeedUp),
                std::make_pair(Ability::specialSaver, Ability::swimSpeedUp),
                std::make_pair(Ability::swimSpeedUp, Ability::swimSpeedUp),
                std::make_pair(Ability::runSpeedUp, Ability::swimSpeedUp),
                std::make_pair(Ability::swimSpeedUp, Ability::swimSpeedUp),
            }
        ),
    };

    for (const auto& brandName: neutralBrands) {
        for (const auto& [expectedResults, abilitiesAndDrinks]: testCases) {
            SeedHelper seedHelper{brandName};

            RollSequence rollSequence{};
            for (const auto& [ability, drink]: abilitiesAndDrinks) {
                rollSequence.addRoll(ability, drink);
            }

            const auto results = seedHelper.findSeed(rollSequence);
            EXPECT_EQ(results.size(), expectedResults.size()) << "Test case: (" << brandName << ", " << ::testing::PrintToString(expectedResults) << ")";

            const std::unordered_set<uint32_t> resultsSet(results.begin(), results.end());
            const std::unordered_set<uint32_t> expectedResultsSet(expectedResults.begin(), expectedResults.end());
            EXPECT_EQ(resultsSet, expectedResultsSet) << "Test case: (" << brandName << ", " << ::testing::PrintToString(expectedResults) << ")";
        }
    }
}


TEST(SeedHelperTest, FindSeedBiasedBrandsWithDrink) {
    /// (brand, expected results/initial seeds, (rolled ability, drink))
    const std::vector<std::tuple<std::string_view, std::vector<uint32_t>, std::vector<std::pair<Ability, Ability>>>> testCases {
        // Likely ability
        std::make_tuple(
            "Skalop",
            std::vector<uint32_t>{0xa55a5ab8, 0x499b9d8e},
            std::vector<std::pair<Ability, Ability>>{
                std::make_pair(Ability::inkRecoveryUp, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::inkSaverMain, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::intensifyAction, Ability::quickRespawn),
                std::make_pair(Ability::subResistanceUp, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::quickSuperJump, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
            }
        ),
        std::make_tuple(
            "Skalop",
            std::vector<uint32_t>{0xd5ab79e8},
            std::vector<std::pair<Ability, Ability>>{
                std::make_pair(Ability::inkSaverMain, Ability::noDrink),
                std::make_pair(Ability::intensifyAction, Ability::noDrink),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::subResistanceUp, Ability::quickRespawn),
                std::make_pair(Ability::runSpeedUp, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::quickRespawn, Ability::quickRespawn),
                std::make_pair(Ability::quickSuperJump, Ability::quickRespawn),
                std::make_pair(Ability::subResistanceUp, Ability::quickRespawn),
            }
        ),

        // Unlikely ability
        std::make_tuple(
            "SquidForce",
            std::vector<uint32_t>{0x75068636},
            std::vector<std::pair<Ability, Ability>>{
                std::make_pair(Ability::inkSaverMain, Ability::inkSaverMain),
                std::make_pair(Ability::inkResistanceUp, Ability::inkSaverMain),
                std::make_pair(Ability::inkSaverMain, Ability::inkSaverMain),
                std::make_pair(Ability::inkSaverMain, Ability::inkSaverMain),
                std::make_pair(Ability::inkSaverMain, Ability::inkSaverMain),
                std::make_pair(Ability::specialPowerUp, Ability::inkSaverMain),
                std::make_pair(Ability::quickSuperJump, Ability::inkSaverMain),
                std::make_pair(Ability::inkSaverMain, Ability::inkSaverMain),
                std::make_pair(Ability::specialSaver, Ability::inkSaverMain),
                std::make_pair(Ability::inkSaverMain, Ability::inkSaverMain),
                std::make_pair(Ability::subResistanceUp, Ability::inkSaverMain),
            }
        ),
        std::make_tuple(
            "SquidForce",
            std::vector<uint32_t>{0xfd01460a, 0xae7aac07},  // After 2 rolls: {0xace3d296, 0xa802cdef}
            std::vector<std::pair<Ability, Ability>>{
                std::make_pair(Ability::quickRespawn, Ability::noDrink),
                std::make_pair(Ability::intensifyAction, Ability::noDrink),
                std::make_pair(Ability::specialPowerUp, Ability::inkSaverMain),
                std::make_pair(Ability::inkSaverMain, Ability::inkSaverMain),
                std::make_pair(Ability::inkSaverMain, Ability::inkSaverMain),
                std::make_pair(Ability::inkSaverMain, Ability::inkSaverMain),
                std::make_pair(Ability::subResistanceUp, Ability::inkSaverMain),
                std::make_pair(Ability::inkRecoveryUp, Ability::inkSaverMain),
                std::make_pair(Ability::quickSuperJump, Ability::inkSaverMain),
            }
        ),

        // Neutral ability
        std::make_tuple(
            "Takoroka",
            std::vector<uint32_t>{},  // No possible seed.
            std::vector<std::pair<Ability, Ability>>{
                std::make_pair(Ability::inkRecoveryUp, Ability::runSpeedUp),
                std::make_pair(Ability::quickRespawn, Ability::runSpeedUp),
                std::make_pair(Ability::specialSaver, Ability::runSpeedUp),
                std::make_pair(Ability::subResistanceUp, Ability::runSpeedUp),
                std::make_pair(Ability::inkSaverSub, Ability::runSpeedUp),
                std::make_pair(Ability::runSpeedUp, Ability::runSpeedUp),
                std::make_pair(Ability::quickSuperJump, Ability::runSpeedUp),
                std::make_pair(Ability::specialPowerUp, Ability::runSpeedUp),
                std::make_pair(Ability::subPowerUp, Ability::runSpeedUp),
                std::make_pair(Ability::quickRespawn, Ability::runSpeedUp),
            }
        ),
        std::make_tuple(
            "Takoroka",
            std::vector<uint32_t>{0xdc3f06a8},
            std::vector<std::pair<Ability, Ability>>{
                std::make_pair(Ability::inkSaverSub, Ability::runSpeedUp),
                std::make_pair(Ability::runSpeedUp, Ability::runSpeedUp),
                std::make_pair(Ability::inkRecoveryUp, Ability::runSpeedUp),
                std::make_pair(Ability::runSpeedUp, Ability::runSpeedUp),
                std::make_pair(Ability::inkResistanceUp, Ability::runSpeedUp),
                std::make_pair(Ability::runSpeedUp, Ability::runSpeedUp),
                std::make_pair(Ability::specialChargeUp, Ability::runSpeedUp),
                std::make_pair(Ability::inkResistanceUp, Ability::runSpeedUp),
                std::make_pair(Ability::inkRecoveryUp, Ability::runSpeedUp),
                std::make_pair(Ability::subResistanceUp, Ability::runSpeedUp),
            }
        ),
    };

    for (const auto& [brandName, expectedResults, abilitiesAndDrinks]: testCases) {
        SeedHelper seedHelper{brandName};

        RollSequence rollSequence{};
        for (const auto& [ability, drink]: abilitiesAndDrinks) {
            rollSequence.addRoll(ability, drink);
        }

        const auto results = seedHelper.findSeed(rollSequence);
        EXPECT_EQ(results.size(), expectedResults.size()) << "Test case: (" << brandName << ", " << ::testing::PrintToString(expectedResults) << ")";

        const std::unordered_set<uint32_t> resultsSet(results.begin(), results.end());
        const std::unordered_set<uint32_t> expectedResultsSet(expectedResults.begin(), expectedResults.end());
        EXPECT_EQ(resultsSet, expectedResultsSet) << "Test case: (" << brandName << ", " << ::testing::PrintToString(expectedResults) << ")";
    }
}


#pragma mark Split seed range
TEST(SeedHelperTest, DoWhile) {  // `findSeedWorker` loop.
    const uint32_t seedStart = UINT32_MAX - 2;
    const uint32_t seedStop = UINT32_MAX;
    const std::vector<uint32_t> expectedResults{UINT32_MAX - 2, UINT32_MAX - 1, UINT32_MAX};
    std::vector<uint32_t> results{};

    uint32_t initialSeed = seedStart;
    do {
        results.push_back(initialSeed);
    } while (initialSeed++ != seedStop);

    EXPECT_EQ(expectedResults, results);
}


TEST(SeedHelperTest, SplitSeedRange) {
    for (size_t segmentsCount = 1; segmentsCount <= 10000; segmentsCount += 1) {
        std::vector<std::pair<uint32_t, uint32_t>> segments{};
        segments.reserve(segmentsCount);

        // Generate
        for (size_t i = 0; i < (segmentsCount - 1); i += 1) {
            const uint32_t start = UINT32_MAX / segmentsCount * i;
            const uint32_t stop = UINT32_MAX / segmentsCount * (i + 1) - 1;
            segments.emplace_back(start, stop);
        }
        if (segmentsCount > 0) {
            const uint32_t finalStart = UINT32_MAX / segmentsCount * (segmentsCount - 1);
            segments.emplace_back(finalStart, UINT32_MAX);
        }

        // Verify
        uint64_t currentNum = 0;
        for (const auto [start, stop]: segments) {
            EXPECT_EQ(currentNum, start) << "Segments count: " << segmentsCount;
            currentNum = static_cast<uint64_t>(stop) + 1;
        }
        EXPECT_EQ(currentNum - 1, UINT32_MAX) << "Doesn't end on `UINT32_MAX`. Segments count: " << segmentsCount;
    }
}
