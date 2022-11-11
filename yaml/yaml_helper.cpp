//
// Created by Jim Shi on 2022-11-06.
//

#include "yaml_helper.h"

#include <fstream>
#include <filesystem>

#include "yaml-cpp/yaml.h"


void YamlFile::setInitialSeed(uint32_t seed) {
    initialSeed = seed;
    dirty = true;
}

void YamlFile::addRoll(Ability ability) {
    rollSequence.addRoll(ability);
    dirty = true;
}

void YamlFile::addRoll(Ability ability, Ability drink) {
    rollSequence.addRoll(ability, drink);
    dirty = true;
}

void YamlFile::load() {
    auto root = YAML::LoadFile(filename);

    if (root["name"]) {
        name = root["name"].as<std::string>();
    } else {
        throw std::runtime_error("`name` absent from YAML file " + filename);
    }

    if (root["brand"]) {
        brand = root["brand"].as<std::string>();
    } else {
        throw std::runtime_error("`brand` absent from YAML file " + filename);
    }

    if (root["initial_seed"]) {
        initialSeed = root["initial_seed"].as<uint32_t>();
    }

    const auto& abilitiesNode = root["abilities"];
    if (abilitiesNode) {
        for (YAML::const_iterator it = abilitiesNode.begin(); it != abilitiesNode.end(); it ++) {
            if (it->IsMap()) {
                // Ability and drink.
                const auto abilityNode = (*it)["ability"];
                if (!abilityNode) {
                    throw std::runtime_error("No ability key in map.");
                }
                const auto abilityId = abilityNode.as<std::string>();
                const auto ability = AbilityHelper::fromId(abilityId);

                const auto drinkNode = (*it)["drink"];
                if (drinkNode) {
                    // Drink used.
                    const auto drinkId = drinkNode.as<std::string>();
                    const auto drink = AbilityHelper::fromId(drinkId);
                    rollSequence.addRoll(ability, drink);
                } else {
                    // No drink used.
                    rollSequence.addRoll(ability);
                }
            } else {
                // Ability only.
                const auto abilityId = it->as<std::string>();
                const auto ability = AbilityHelper::fromId(abilityId);
                rollSequence.addRoll(ability);
            }
        }
    }
}

void YamlFile::saveIfDirty() {
    if (!dirty) {
        return;
    }

    YAML::Node root{};
    root["name"] = name;
    root["brand"] = brand;
    if (initialSeed.has_value()) {
        root["initial_seed"] = initialSeed.value();
    }
    if (!rollSequence.empty()) {
        for (const auto [ability, drink]: rollSequence) {
            const std::string abilityId{AbilityHelper::getId(ability)};
            if (drink == Ability::noDrink) {
                root["abilities"].push_back(abilityId);
            } else {
                const std::string drinkId{AbilityHelper::getId(drink)};

                YAML::Node currentNode{};
                currentNode["ability"] = abilityId;
                currentNode["drink"] = drinkId;
                root["abilities"].push_back(currentNode);
            }
        }
    }

    std::ofstream f(filename);
    f << root;
    f.close();

    dirty = false;
}

YamlFile::YamlFile(std::string_view filename): filename{filename}, dirty{false}, name{}, brand{}, initialSeed{}, rollSequence{} {
    if (std::filesystem::is_regular_file(filename)) {
        load();
    } else {
        throw std::runtime_error("YAML file doesn't exist.");
    }
}

YamlFile::~YamlFile() {
    saveIfDirty();
}



