//
// Created by Jim Shi on 2022-11-09.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_ABILITY_H
#define SPLATOON_3_GEAR_HELPER_CPP_ABILITY_H

#include <array>
#include <string_view>


enum class Ability: size_t {
    inkSaverMain,
    inkSaverSub,
    inkRecoveryUp,
    runSpeedUp,
    swimSpeedUp,
    specialChargeUp,
    specialSaver,
    specialPowerUp,
    quickRespawn,
    quickSuperJump,
    subPowerUp,
    inkResistanceUp,
    subResistanceUp,
    intensifyAction,

    /// Placeholder: Can be any ability.
    unknown,

    /// No drink used.
    noDrink,
};


namespace AbilityHelper {
    constexpr std::array<std::string_view, 14> ids = {
        "ink_saver_main",
        "ink_saver_sub",
        "ink_recovery_up",
        "run_speed_up",
        "swim_speed_up",
        "special_charge_up",
        "special_saver",
        "special_power_up",
        "quick_respawn",
        "quick_super_jump",
        "sub_power_up",
        "ink_resistance_up",
        "sub_resistance_up",
        "intensify_action",
    };
    constexpr std::string_view placeholderId = "unknown";

    Ability fromId(std::string_view id);

    /// Get ability ID.
    std::string_view getId(Ability ability);

    /// Get ability index.
    inline size_t getIndex(Ability ability) {
        return static_cast<size_t>(ability);
    }
}


#endif //SPLATOON_3_GEAR_HELPER_CPP_ABILITY_H
