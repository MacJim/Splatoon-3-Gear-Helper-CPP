//
// Created by Jim Shi on 2022-11-10.
//

#include "gtest/gtest.h"

#include "../data/ability.h"


using namespace AbilityHelper;


TEST(AbilityHelperTest, FromId) {
    for (size_t i = 0; i < ids.size(); i += 1) {
        auto ability = fromId(ids[i]);
        EXPECT_EQ(ability, static_cast<Ability>(i));
    }

    auto placeholderAbility = fromId(placeholderId);
    EXPECT_EQ(placeholderAbility, Ability::unknown);
}


TEST(AbilityHelperTest, GetId) {
    for (size_t i = 0; i < ids.size(); i += 1) {
        auto ability = static_cast<Ability>(i);
        EXPECT_EQ(getId(ability), ids[i]);
    }

    auto placeholderAbility = Ability::unknown;
    EXPECT_EQ(getId(placeholderAbility), placeholderId);
}


TEST(AbilityHelperTest, GetIndex) {
    for (size_t i = 0; i < ids.size(); i += 1) {
        auto ability = static_cast<Ability>(i);
        EXPECT_EQ(getIndex(ability), i);
    }
}
