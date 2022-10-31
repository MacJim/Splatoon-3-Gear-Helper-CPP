#include <array>
#include <unordered_map>

#include "gtest/gtest.h"

#include "../seed_helper.h"
#include "../data/abilities.h"
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


#pragma mark findSeed
