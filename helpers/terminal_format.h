//
// Created by Jim Shi on 5/27/21.
//

#ifndef TERMINAL_FORMAT_H
#define TERMINAL_FORMAT_H

#include <iostream>

namespace TerminalFormat {
    using namespace std::literals;

    constexpr auto HEADER = "\033[95m"sv;
    constexpr auto OK_BLUE = "\033[94m"sv;
    constexpr auto OK_CYAN = "\033[96m"sv;
    constexpr auto OK_GREEN = "\033[92m"sv;
    constexpr auto WARNING = "\033[93m"sv;
    constexpr auto FAIL = "\033[91m"sv;
    constexpr auto ENDC = "\033[0m"sv;
    constexpr auto BOLD = "\033[1m"sv;
    constexpr auto UNDERLINE = "\033[4m"sv;
}

#endif //TERMINAL_FORMAT_H
