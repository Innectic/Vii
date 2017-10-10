#pragma once

#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>


#ifdef _WIN32
#define int64 __int64
#endif
#ifdef __linux__
#include <inttypes.h>
#define int64 int64_t
#endif

class Util {
public:
    const static inline std::string eatSpaces(std::string original) {
        return std::regex_replace(original, std::regex("^ +| +$|( ) +"), "$1");
    }
    
    const static inline std::vector<std::string> readFileToVector(const std::string& file) {
        std::vector<std::string> lines;
        
        std::ifstream input;
        input.open(file);

        if (!input) return lines;

        std::string current = "";
        while (std::getline(input, current)) {
            lines.emplace_back(current);
        }
        input.close();
        return lines;
    }

    const static inline std::string join(const std::vector<std::string> joining) {
        std::string result;
        for (auto join : joining) result += join;
        return result;
    }

    const static inline bool is_number(std::string check) {
        bool allMatch = true;
        for (auto c : check) allMatch = is_number(c);
        return allMatch;
    }

    const static inline bool is_number(char& check) {
        return isdigit(check);
    }

    // TODO: This is pretty bad, since it only works on the one type.
    const static inline bool vectorContains(std::vector<std::string> vec, std::string check) {
        for (auto in : vec) if (in == check) return true;
        return false;
    }

    const static inline std::string string_to_lower(std::string original) {
        std::transform(original.begin(), original.end(), original.begin(), ::tolower);
        return original;
    }

    const static inline std::string trim_string(std::string original) {
        const std::string pattern = " \f\n\r\t\v";

        auto new_string = original.substr(0, original.find_last_not_of(pattern) + 1);
        new_string = new_string.substr(new_string.find_first_not_of(pattern));
        return new_string;
    }

    const static inline int64 get_time() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    const static inline std::string replace(const std::string& original, const std::string& replace, const std::string& with) {
        std::string replaced = original;
        replaced = std::regex_replace(replaced, std::regex(replace), with);
        return replaced;
    }
};
