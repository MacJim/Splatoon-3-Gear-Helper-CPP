#include <iostream>

#include "seed_helper.h"
#include "data/roll_sequence.h"


void find(const std::string_view& brandName, const std::vector<std::string_view>& previousRolls) {
    SeedHelper seedHelper(brandName);
    RollSequence rollSequence(previousRolls);
    const auto results = seedHelper.findSeed(rollSequence, 8);
    if (!results.empty()) {
        for (const auto& result: results) {
            std::cout << std::hex << "0x" << result << "\n";
        }
    }
}


void predict() {
    // TODO:
}


namespace ErrorCode {
    constexpr int NO_COMMAND = 1;
    constexpr int INVALID_COMMAND = 2;
    constexpr int NOT_ENOUGH_ARGUMENTS = 3;
    constexpr int NOT_IMPLEMENTED = 128;
}


int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "No command given." << std::endl;
        return ErrorCode::NO_COMMAND;
    }

    const std::string_view command {argv[1]};
    if (command == "find") {
        if (argc < 4) {
            std::cout << "Not enough arguments." << std::endl;
            return ErrorCode::NOT_ENOUGH_ARGUMENTS;
        }

        const std::string_view brandName {argv[2]};

        std::vector<std::string_view> previousRolls {};
        for (int i = 3; i < argc; i += 1) {
            previousRolls.emplace_back(argv[i]);
        }

        find(brandName, previousRolls);
    } else if (command == "predict") {
        std::cout << "Not implemented." << std::endl;
        return ErrorCode::NOT_IMPLEMENTED;
    } else {
        std::cout << "Invalid command `" << command << "`." << std::endl;
        return ErrorCode::INVALID_COMMAND;
    }

    return 0;
}
