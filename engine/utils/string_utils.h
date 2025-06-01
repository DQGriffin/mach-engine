//
// Created by Dquavius Griffin on 6/1/25.
//

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>
#include <sstream>

namespace mach::utils
{
    inline std::vector<std::string> split_str(const std::string& input, char delimiter)
    {
        std::vector<std::string> tokens;
        std::istringstream stream(input);
        std::string token;

        while (std::getline(stream, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }
}

#endif //STRING_UTILS_H
