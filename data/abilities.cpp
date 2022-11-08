//
// Created by Jim Shi on 2022-11-04.
//

#include "abilities.h"

#include <string>
#include <exception>


size_t getAbilityIndex(const std::string_view ability) {
    for (size_t i = 0; i < ability.size(); i += 1) {
        if (ability == abilities[i]) {
            return i;
        }
    }

    std::string errorMessage{"Invalid ability "};
    errorMessage += ability;
    throw std::runtime_error(errorMessage);
}
