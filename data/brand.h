//
// Created by Jim Shi on 2022-10-28.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_BRANDS_H
#define SPLATOON_3_GEAR_HELPER_CPP_BRANDS_H

#include <array>
#include <string_view>
#include <tuple>

#include "ability.h"


constexpr std::array<std::string_view, 3> neutralBrands = {
    "Amiibo",
    "Cuttlegear",
    "Grizzco",
};

/// (brand name, likely ability, unlikely ability)
constexpr std::array<std::tuple<std::string_view, Ability, Ability>, 17> biasedBrands = {
    std::make_tuple("Annaki", Ability::inkSaverSub, Ability::specialSaver),
    std::make_tuple("Barazushi", Ability::intensifyAction, Ability::subPowerUp),
    std::make_tuple("Emberz", Ability::intensifyAction, Ability::specialChargeUp),
    std::make_tuple("Enperry", Ability::subPowerUp, Ability::inkResistanceUp),
    std::make_tuple("Firefin", Ability::inkSaverSub, Ability::inkRecoveryUp),
    std::make_tuple("Forge", Ability::specialPowerUp, Ability::inkSaverSub),
    std::make_tuple("Inkline", Ability::subResistanceUp, Ability::intensifyAction),
    std::make_tuple("Krak-On", Ability::swimSpeedUp, Ability::subResistanceUp),
    std::make_tuple("Rockenberg", Ability::runSpeedUp, Ability::swimSpeedUp),
    std::make_tuple("Skalop", Ability::quickRespawn, Ability::specialSaver),
    std::make_tuple("Splash Mob", Ability::inkSaverMain, Ability::runSpeedUp),
    std::make_tuple("SquidForce", Ability::inkResistanceUp, Ability::inkSaverMain),
    std::make_tuple("Takoroka", Ability::specialChargeUp, Ability::specialPowerUp),
    std::make_tuple("Tentatek", Ability::inkRecoveryUp, Ability::quickSuperJump),
    std::make_tuple("Toni Kensa", Ability::inkSaverMain, Ability::subPowerUp),
    std::make_tuple("Zekko", Ability::specialSaver, Ability::specialChargeUp),
    std::make_tuple("Zink", Ability::quickSuperJump, Ability::quickRespawn),
};


#endif //SPLATOON_3_GEAR_HELPER_CPP_BRANDS_H
