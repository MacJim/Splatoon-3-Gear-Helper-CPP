//
// Created by Jim Shi on 2022-11-09.
//

#include "ability.h"

#include <string>


namespace AbilityHelper {
    Ability fromId(std::string_view id) {
        for (size_t i = 0; i < ids.size(); i += 1) {
            if (id == ids[i]) {
                return Ability(i);
            }
        }

        if (id == placeholderId) {
            return Ability::unknown;
        } else {
            std::string exceptionMessage{"Invalid ability ID: "};
            exceptionMessage += id;
            throw std::invalid_argument(exceptionMessage);
        }
    }

    std::string_view getId(Ability ability) {
        switch (ability) {
            case Ability::noDrink:
                return std::string_view{};
            case Ability::unknown:
                return placeholderId;
            default:
                return ids[getIndex(ability)];
        }
    }
}
