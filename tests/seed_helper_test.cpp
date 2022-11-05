#include <array>
#include <unordered_map>
#include <set>

#include "gtest/gtest.h"

#include "../seed_helper.h"
#include "../data/brands.h"


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


#pragma mark getBrandedAbility
TEST(SeedHelperTest, GetBrandedAbilityNeutralBrands) {
    /// (seed, expected result)
    constexpr std::array<std::pair<uint32_t, std::string_view>, 22> testCases = {
        std::make_pair(0x0, "ink_saver_main"),
        std::make_pair(0x42021, "ink_saver_main"),
        std::make_pair(0x84042, "ink_saver_sub"),
        std::make_pair(0xc6063, "ink_saver_sub"),
        std::make_pair(0x108084, "ink_recovery_up"),
        std::make_pair(0x14a0a5, "ink_recovery_up"),
        std::make_pair(0x18c0c6, "run_speed_up"),
        std::make_pair(0x1ce0e7, "run_speed_up"),

        // Obtained from the seed sequence starting with 0x1.
        std::make_pair(0x60b66de0, "ink_saver_main"),
        std::make_pair(0xcf2eecfa, "ink_saver_sub"),
        std::make_pair(0xb945bae9, "ink_recovery_up"),
        std::make_pair(0x4eb10b7, "run_speed_up"),
        std::make_pair(0x13223391, "swim_speed_up"),
        std::make_pair(0x52f4d3eb, "special_charge_up"),
        std::make_pair(0x9f2de485, "special_saver"),
        std::make_pair(0xa956130a, "special_power_up"),
        std::make_pair(0xdcc65bd4, "quick_respawn"),
        std::make_pair(0x379686c7, "quick_super_jump"),
        std::make_pair(0xc651068d, "sub_power_up"),
        std::make_pair(0x19f91cb2, "ink_resistance_up"),
        std::make_pair(0xddbb4b1, "sub_resistance_up"),
        std::make_pair(0x591c9737, "intensify_action"),
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
    const std::unordered_map<std::string_view, std::array<std::pair<uint32_t, std::string_view>, 3>> testCases = {
        {"Annaki", {
            std::make_pair(0x887e9a54, "ink_saver_sub"),
            std::make_pair(0x36cc0f25, "special_saver"),
            std::make_pair(0x436dfc0, "ink_recovery_up"),
        }},
        {"Barazushi", {
           std::make_pair(0x8b3644cf, "intensify_action"),
           std::make_pair(0x4eab4f2f, "sub_power_up"),
           std::make_pair(0x39d2b193, "quick_respawn"),
        }},
        {"Emberz", {
           std::make_pair(0x4111a2fd, "intensify_action"),
           std::make_pair(0x94edd48f, "special_charge_up"),
           std::make_pair(0xff675892, "run_speed_up"),
        }},
        {"Enperry", {
           std::make_pair(0xe90c15d3, "sub_power_up"),
           std::make_pair(0xf6bd5e1f, "ink_resistance_up"),
           std::make_pair(0x5a77d10, "swim_speed_up"),
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


#pragma mark generateRoll
TEST(SeedHelperTest, GenerateRollNeutralBrands) {
    constexpr uint32_t initialSeed = 0x81207561;
    constexpr std::array<std::pair<uint32_t, std::string_view>, 7> expectedResults = {
        std::make_pair(0x7e0e4647, "special_power_up"),
        std::make_pair(0x6e1959a4, "swim_speed_up"),
        std::make_pair(0xe0929d72, "intensify_action"),
        std::make_pair(0xd4ac196c, "swim_speed_up"),
        std::make_pair(0xa7b7e72c, "sub_resistance_up"),
        std::make_pair(0x311b1a25, "special_saver"),
        std::make_pair(0x19adf24a, "run_speed_up"),
    };

    for (auto& brandName: neutralBrands) {
        auto seedHelper = SeedHelper(brandName);

        uint32_t seed = initialSeed;
        for (auto& [expectedSeed, expectedAbility]: expectedResults) {
            const auto& result = seedHelper.generateRoll(seed);
            seed = result.first;
            EXPECT_EQ(seed, expectedSeed) << "Test case: (" << brandName << ", 0x" << std::hex << initialSeed << ")";
            EXPECT_EQ(result.second, expectedAbility) << "Test case: (" << brandName << ", 0x" << std::hex << initialSeed << ")";
        }
    }
}


TEST(SeedHelperTest, GenerateRollBiasedBrands) {
    /**
     * {brand name: (seed, expected next seed, expected result)}
     * Order: Likely, unlikely, neutral
     */
    const std::unordered_map<std::string_view, std::array<std::tuple<uint32_t, uint32_t, std::string_view>, 3>> testCases = {
        {"Krak-On", {
            std::make_tuple(0xa5b4a2ae, 0xdce319e, "swim_speed_up"),
            std::make_tuple(0x14415c0a, 0xc7c07eaa, "sub_resistance_up"),
            std::make_tuple(0x581853e9, 0x1ece01bb, "quick_respawn"),
        }},
        {"Splash Mob", {
            std::make_tuple(0x10b8c0fe, 0x1c47910d, "ink_saver_main"),
            std::make_tuple(0x236779ee, 0x470127a3, "run_speed_up"),
            std::make_tuple(0x850f3db7, 0x3584718b, "ink_recovery_up"),
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


#pragma mark generateRollWithDrink
TEST(SeedHelperTest, GenerateRollWithDrinkNeutralBrands) {
    constexpr uint32_t initialSeed = 0x79239fed;
    constexpr std::string_view drink = "run_speed_up";
    /// (expected seed, expected result)
    constexpr std::array<std::pair<uint32_t, std::string_view>, 10> expectedResults = {
        std::make_pair(0x51196ac2, "run_speed_up"),
        std::make_pair(0x2b86c939, "sub_resistance_up"),
        std::make_pair(0xa6939d49, "run_speed_up"),
        std::make_pair(0x8e984091, "sub_power_up"),
        std::make_pair(0x7673e6, "ink_resistance_up"),
        std::make_pair(0xce455fc3, "swim_speed_up"),
        std::make_pair(0xd21caebd, "run_speed_up"),
        std::make_pair(0xa2d39c2a, "sub_power_up"),
        std::make_pair(0xfb8024a1, "run_speed_up"),
        std::make_pair(0xb41a6d22, "special_power_up"),
    };

    for (auto& brandName: neutralBrands) {
        auto seedHelper = SeedHelper(brandName);
        seedHelper.cacheAllDrinkWeightsMaps();

        uint32_t seed = initialSeed;
        for (auto& [expectedSeed, expectedAbility]: expectedResults) {
            const auto& result = seedHelper.generateRollWithDrink(seed, drink);
            seed = result.first;
            EXPECT_EQ(seed, expectedSeed) << "Test case: (" << brandName << ", 0x" << std::hex << initialSeed << ")";
            EXPECT_EQ(result.second, expectedAbility) << "Test case: (" << brandName << ", 0x" << std::hex << initialSeed << ")";
        }
    }
}


TEST(SeedHelperTest, GenerateRollWithDrinkBiasedBrands) {
    using ExpectedSequence = std::vector<std::pair<uint32_t, std::string_view>>;
    /// (brand name, initial seed, drink, (expected seed, expected ability))
    const std::vector<std::tuple<std::string_view, uint32_t, std::string_view, ExpectedSequence>> testCases {
        std::make_tuple("Forge", 0x17239048, "quick_respawn", ExpectedSequence{
            std::make_pair(0xf57b8c75, "intensify_action"),
            std::make_pair(0x1a58afef, "quick_respawn"),
            std::make_pair(0xfb0c4f9d, "quick_respawn"),
            std::make_pair(0x2d050aa2, "quick_respawn"),
            std::make_pair(0x6709dca, "quick_respawn"),
            std::make_pair(0x9f7c6099, "intensify_action"),
            std::make_pair(0x7e866cee, "quick_respawn"),
            std::make_pair(0x13e59081, "special_saver"),
            std::make_pair(0xf7f1178e, "sub_resistance_up"),
            std::make_pair(0x475c66a9, "swim_speed_up"),
        }),
        std::make_tuple("Inkline", 0x231897b1, "ink_saver_sub", ExpectedSequence{
            std::make_pair(0xc3b4786, "ink_saver_sub"),
            std::make_pair(0xcd25bf65, "sub_resistance_up"),
            std::make_pair(0x23ed6221, "ink_saver_sub"),
            std::make_pair(0xf1848924, "sub_resistance_up"),
            std::make_pair(0x74a717f4, "ink_saver_sub"),
            std::make_pair(0x5d6247d8, "ink_saver_sub"),
            std::make_pair(0xa6b0ef94, "ink_saver_sub"),
            std::make_pair(0xef49b19e, "special_saver"),
            std::make_pair(0xf639b943, "ink_saver_sub"),
            std::make_pair(0x554cdf72, "ink_saver_main"),
        }),
        std::make_tuple("Rockenberg", 0xa8b97928, "sub_power_up", ExpectedSequence{
            std::make_pair(0x32ae369, "ink_saver_sub"),
            std::make_pair(0xb7ba758a, "sub_power_up"),
            std::make_pair(0x3a878850, "run_speed_up"),
            std::make_pair(0xba305f56, "sub_power_up"),
            std::make_pair(0x5fbfe477, "ink_resistance_up"),
            std::make_pair(0xbb50d442, "run_speed_up"),
            std::make_pair(0x9ac0516e, "sub_power_up"),
            std::make_pair(0x10afb152, "quick_respawn"),
            std::make_pair(0x48c7666b, "run_speed_up"),
            std::make_pair(0x2540d9ae, "sub_power_up"),
        }),
    };

    for (const auto& [brandName, initialSeed, drink, brandTestCases]: testCases) {
        auto seedHelper = SeedHelper(brandName);
        seedHelper.cacheDrinkWeightsMap(drink);

        uint32_t seed = initialSeed;
        for (auto& [expectedNextSeed, expectedAbility]: brandTestCases) {
            const auto& result = seedHelper.generateRollWithDrink(seed, drink);
            seed = result.first;
            EXPECT_EQ(result.first, expectedNextSeed) << "Test case: (" << brandName << ", 0x" << std::hex << seed << ", 0x" << expectedNextSeed << ")";
            EXPECT_EQ(result.second, expectedAbility) << "Test case: (" << brandName << ", 0x" << std::hex << seed << ", 0x" << expectedNextSeed << ")";
        }
    }
}


#pragma mark findSeed, no drink
TEST(SeedHelperTest, FindSeedNeutralBrandsNoDrink) {
    /// (expected results/initial seed, rolled abilities)
    const std::vector<std::pair<std::vector<uint32_t>, std::vector<std::string_view>>> testCases = {
        std::make_pair(
            std::vector<uint32_t>({0x87a4b37e}),
            std::vector<std::string_view>({"run_speed_up", "special_charge_up", "special_charge_up", "sub_power_up", "run_speed_up", "special_saver", "special_charge_up", "intensify_action", "special_charge_up", "special_saver", "special_charge_up", "special_saver", "swim_speed_up", "special_saver", "special_saver"})
        ),
        std::make_pair(
            std::vector<uint32_t>({0x3b09c31b, 0xb32ef1ef, 0xd929930b, 0xea2db1fe}),
            std::vector<std::string_view>({"ink_saver_sub", "ink_resistance_up", "special_power_up", "ink_saver_sub", "ink_recovery_up", "sub_power_up", "run_speed_up", "special_power_up"})
        ),
        std::make_pair(
            std::vector<uint32_t>({0x6190ed, 0x9778b27, 0xad66e54, 0xe4d3afa, 0x12e71833, 0x14054d6b, 0x17e85407, 0x1bbfa76e, 0x1c20f670, 0x22b76a45, 0x26ccf63f, 0x2931987c, 0x2ac8b6c2, 0x2b45e545, 0x2c3341f1, 0x2cbff1ff, 0x38bd30f2, 0x42e9f7fd, 0x498958ec, 0x51de83ff, 0x766cd6ba, 0x78cf83f3, 0x89ed9033, 0x8b687e8f, 0x8c465ea7, 0x8dba02ed, 0x91238fad, 0x94fe7fd0, 0xa268c374, 0xa3440b6b, 0xaa358c67, 0xad1ad783, 0xb21c2cff, 0xbc9ba1e5, 0xd4a5f808, 0xda744104, 0xe425c810, 0xeecadc56, 0xefabfaa1, 0xf452fc06, 0xf63dcb84}),
            std::vector<std::string_view>({"ink_saver_sub", "quick_respawn", "ink_recovery_up", "ink_saver_main", "special_charge_up", "special_power_up", "special_saver"})
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
    const std::vector<std::tuple<std::string_view, std::vector<uint32_t>, std::vector<std::string_view>>> testCases = {
        std::make_tuple(
            "Zekko",
            std::vector<uint32_t>({0x87b091}),
            std::vector<std::string_view>({"sub_resistance_up", "special_saver", "quick_super_jump", "swim_speed_up", "special_power_up", "quick_respawn", "quick_super_jump", "ink_saver_main", "ink_saver_sub", "ink_saver_sub"})
        ),
        std::make_tuple(
            "Firefin",
            std::vector<uint32_t>({0x2ff2311e, 0x5cf03863, 0x809b1a97}),
            std::vector<std::string_view>({"ink_saver_sub", "quick_respawn", "sub_resistance_up", "swim_speed_up", "ink_resistance_up", "quick_respawn", "ink_saver_main", "swim_speed_up"})
        ),
        std::make_tuple(
            "Splash Mob",
            std::vector<uint32_t>({0x7843744, 0x1b701a85, 0x21c98a86, 0x351c815a, 0x3810b1d3, 0x3cc89170, 0x58c56bc9, 0x91882513, 0x9ba02391, 0xa55de1c5, 0xbea8715e, 0xbf274b0d, 0xf6249c6c}),
            std::vector<std::string_view>({"swim_speed_up", "ink_saver_sub", "quick_super_jump", "ink_saver_main", "ink_saver_main", "sub_resistance_up", "ink_saver_main", "sub_power_up", "ink_saver_main"})
        ),
    };

    for (auto& [brandName, expectedResults, rolledAbilities]: testCases) {
        const RollSequence rollSequence{rolledAbilities};
        auto seedHelper = SeedHelper(brandName);

        const auto results = seedHelper.findSeed(rollSequence);
        EXPECT_EQ(results.size(), expectedResults.size()) << "Test case: (" << brandName << ", 0x" << std::hex << ::testing::PrintToString(expectedResults) << ")";

        const auto resultsSet = std::set<uint32_t>(results.begin(), results.end());
        const auto expectedResultsSet = std::set<uint32_t>(expectedResults.begin(), expectedResults.end());
        EXPECT_EQ(resultsSet, expectedResultsSet) << "Test case: (" << brandName << ", " << std::hex << ::testing::PrintToString(expectedResults) << ")";
    }
}


#pragma mark findSeed, with drink
// TODO:
