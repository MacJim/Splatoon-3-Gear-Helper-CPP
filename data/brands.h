//
// Created by Jim Shi on 2022-10-28.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_BRANDS_H
#define SPLATOON_3_GEAR_HELPER_CPP_BRANDS_H

#include <array>
#include <string_view>
#include <tuple>


constexpr std::array<std::string_view, 3> neutralBrands = {
    "Amiibo",
    "Cuttlegear",
    "Grizzco",
};

/// (brand name, likely ability, unlikely ability)
constexpr std::array<std::tuple<std::string_view, std::string_view, std::string_view>, 17> biasedBrands = {
    std::make_tuple("Annaki", "ink_saver_sub", "special_saver"),
    std::make_tuple("Barazushi", "intensify_action", "sub_power_up"),
    std::make_tuple("Emberz", "intensify_action", "special_charge_up"),
    std::make_tuple("Enperry", "sub_power_up", "ink_resistance_up"),
    std::make_tuple("Firefin", "ink_saver_sub", "ink_recovery_up"),
    std::make_tuple("Forge", "special_power_up", "ink_saver_sub"),
    std::make_tuple("Inkline", "sub_resistance_up", "intensify_action"),
    std::make_tuple("Krak-On", "swim_speed_up", "sub_resistance_up"),
    std::make_tuple("Rockenberg", "run_speed_up", "swim_speed_up"),
    std::make_tuple("Skalop", "quick_respawn", "special_saver"),
    std::make_tuple("Splash Mob", "ink_saver_main", "run_speed_up"),
    std::make_tuple("SquidForce", "ink_resistance_up", "ink_saver_main"),
    std::make_tuple("Takoroka", "special_charge_up", "special_power_up"),
    std::make_tuple("Tentatek", "ink_recovery_up", "quick_super_jump"),
    std::make_tuple("Toni Kensa", "ink_saver_main", "sub_power_up"),
    std::make_tuple("Zekko", "special_saver", "special_charge_up"),
    std::make_tuple("Zink", "quick_super_jump", "quick_respawn"),
};


#endif //SPLATOON_3_GEAR_HELPER_CPP_BRANDS_H
