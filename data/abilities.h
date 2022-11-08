//
// Created by Jim Shi on 2022-10-27.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_ABILITIES_H
#define SPLATOON_3_GEAR_HELPER_CPP_ABILITIES_H

#include <array>
//#include <unordered_map>
#include <string_view>


constexpr std::array<std::string_view, 14> abilities = {
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

/// Get ability index in `abilities`.
size_t getAbilityIndex(std::string_view ability);

constexpr std::string_view placeholderAbility = "unknown";

//const std::unordered_map<std::string_view, std::string_view> ABILITIES = {
//    {"ink_saver_main", "Ink Saver (Main)"},
//};


//constexpr std::array<std::pair<std::string_view, std::string_view>, 14> abilityIDsAndNames = {
//    std::make_pair("ink_saver_main", "Ink Saver (Main)"),
//};

//constexpr std::array<std::pair<std::string_view, std::string_view>, 14> placeholderAbilityIDsAndNames = {
//    std::make_pair("unknown", "Unknown"),
//};

//std::string_view getAbilityDescription(const std::string_view& abilityID);


#endif //SPLATOON_3_GEAR_HELPER_CPP_ABILITIES_H
