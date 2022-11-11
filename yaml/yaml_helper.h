//
// Created by Jim Shi on 2022-11-06.
//

#ifndef SPLATOON_3_GEAR_HELPER_CPP_YAML_HELPER_H
#define SPLATOON_3_GEAR_HELPER_CPP_YAML_HELPER_H


#include <string>
#include <optional>

#include "../data/roll_sequence.h"


class YamlFile {
#pragma mark File information
private:
    std::string filename;
    bool dirty;

#pragma mark File contents
private:
    std::string name;
    std::string brand;
    std::optional<uint32_t> initialSeed;
    RollSequence rollSequence;

public:
    inline std::string_view getName() {
        return name;
    }
    inline std::string_view getBrand() {
        return brand;
    }
    inline std::optional<uint32_t> getInitialSeed() {
        return initialSeed;
    }
    void setInitialSeed(uint32_t seed);
    void addRoll(Ability ability);
    void addRoll(Ability ability, Ability drink);
    inline const RollSequence& getRollSequence() {
        return rollSequence;
    }

private:
    /// Load file contents.
    void load();

public:
    /// Save file contents if dirty.
    void saveIfDirty();

#pragma mark Constructors
public:
    YamlFile() = delete;
    YamlFile(const YamlFile&) = delete;
    YamlFile& operator=(const YamlFile&) = delete;

    /**
     * Load the contents of `filename` if it exists.
     * Otherwise, load empty file contents, and create the file until `dirty` is set.
     */
    explicit YamlFile(std::string_view filename);

    /**
     * Used in tests to:
     *
     * - Create YAML files from scratch
     * - Test the saveIfDirty functionality
     */
    YamlFile(std::string_view filename, std::string_view name, std::string_view brand, std::optional<uint32_t> initialSeed): filename{filename}, dirty{true}, name{name}, brand{brand}, initialSeed{initialSeed}, rollSequence{} {}

    /// Saves the file.
    ~YamlFile();
};


#endif //SPLATOON_3_GEAR_HELPER_CPP_YAML_HELPER_H
