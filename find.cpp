#include <iostream>
#include <stdexcept>
#include <thread>

#include "yaml/yaml_helper.h"
#include "seed_helper.h"


int main(int argc, char* argv[]) {
    // Parse arguments.
    if (argc == 1) {
        throw std::invalid_argument("No filename given.");
    }

    const auto filename = argv[1];
    bool dryRun = false;

    for (int i = 2; i < argc; i += 1) {
        const std::string_view argument{argv[i]};
        if ((argument == "--dry-run") || (argument == "-n")) {
            dryRun = true;
        } else {
            std::string exceptionMessage{"Unrecognized argument: "};
            exceptionMessage += argument;
            throw std::invalid_argument(exceptionMessage);
        }
    }

    // Load YAML file and predict.
    YamlFile yamlFile{filename};
    if (yamlFile.getRollSequence().empty()) {
        std::string exceptionMessage{"No roll sequence in file: "};
        exceptionMessage += filename;
        throw std::runtime_error(exceptionMessage);
    }

    SeedHelper seedHelper{yamlFile.getBrand()};
    const auto results = seedHelper.findSeed(yamlFile.getRollSequence(), std::thread::hardware_concurrency());
    if (results.empty()) {
        std::cout << "No result found." << std::endl;
        return 1;
    } else if (results.size() > 1) {
        if (results.size() > 100) {
            std::cout << "Too many (" << results.size() << ") results found. Please add more rolls." << std::endl;
        } else {
            std::cout << results.size() << " results found:\n";
            for (const auto result: results) {
                std::cout << "0x" << std::hex << result << "\n";
            }
            std::cout << std::flush;
        }
        return 2;
    }

    // Only 1 possible seed.
    const auto seed = results[0];
    std::cout << "Found seed: 0x" << std::hex << seed << std::endl;
    if (yamlFile.getInitialSeed().has_value()) {
        // Verify existing initial seed.
        if (seed != yamlFile.getInitialSeed()) {
            std::cout << "Doesn't match existing seed in YAML file: 0x" << std::hex << yamlFile.getInitialSeed().value() << std::endl;
            return 3;
        } else {
            std::cout << "Matches seed in YAML file." << std::endl;
        }
    } else {
        // Save the initial seed.
        if (!dryRun) {
            yamlFile.setInitialSeed(seed);
            std::cout << "Initial seed saved to YAML file: " << filename << std::endl;
        }
    }

    return 0;
}
