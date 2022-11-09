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

void YamlFile::addRoll(const std::string_view ability) {
    rollSequence.addRoll(ability);
    dirty = true;
}

void YamlFile::addRoll(std::string_view ability, std::string_view drink) {
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

    if (root["abilities"]) {
        const auto& abilitiesNode = root["abilities"];
        for (YAML::const_iterator it = abilitiesNode.begin(); it != abilitiesNode.end(); it ++) {
            if (it->IsMap()) {
                // Ability and drink.
                // TODO:
                throw std::runtime_error("");
            } else {
                // Ability only.
                // TODO: This temporary string will be destroyed, invalidating `RollSequence`'s `string_view` data.
                const auto ability = it->as<std::string>();
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
            if (drink.empty()) {
                root["abilities"].push_back(ability.data());
            } else {
                YAML::Node currentNode{};
                currentNode["ability"] = ability.data();
                currentNode["drink"] = drink.data();
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



