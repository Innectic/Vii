#pragma once

#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <sstream>

#ifdef _WIN32
#define int64 __int64
#endif
#ifdef __linux__
#include <inttypes.h>
#define int64 int64_t
#endif

class Util {
public:
    const static inline std::vector<std::string> read_file_to_vector(const std::string& file) {
        std::vector<std::string> lines;
        
        std::ifstream input;
        input.open(file);

        if (!input) return lines;

        std::string current = "";
        while (std::getline(input, current)) lines.emplace_back(current);
        input.close();
        return lines;
    }

    const static inline bool is_number(std::string check) {
        bool allMatch = true;
        for (auto& c : check) allMatch = is_number(c);
        return allMatch;
    }

    const static inline bool is_number(char& check) {
        return isdigit(check);
    }

    template<class T>
    const static inline bool vector_contains(std::vector<T> vec, T check) {
        for (auto& in : vec) if (in == check) return true;
        return false;
    }

    const static inline std::string string_to_lower(std::string original) {
        std::transform(original.begin(), original.end(), original.begin(), ::tolower);
        return original;
    }

    const static inline int64 get_time() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    const static inline std::string replace(const std::string& original, const std::string& replace, const std::string& with) {
        std::string replaced = original;
        replaced = std::regex_replace(replaced, std::regex(replace), with);
        return replaced;
    }

    const static bool starts_with(const std::string& check, const std::string& checking) {
        return checking.substr(0, check.size()) == check;
    }

    template<class T, class T2>
    const static bool contains_key(const std::map<T, T2> checking, T key) {
        return checking.find(key) != checking.end();
    }

    const static std::vector<std::string> split(const std::string& splitting, const char& delim, const int count = 1) {
        std::vector<std::string> matches;

        std::string part;
        std::stringstream stream(splitting);
        while (getline(stream, part, delim)) matches.emplace_back(part);

        return matches;
    }

    const static bool string_contains(const std::string& checking, const std::string& contains) {
        return checking.find(contains) != std::string::npos;
    }

	const static std::string trim(const std::string& trimming) {
		auto cleaned = trimming;
		while (cleaned.size() && isspace(cleaned.front())) cleaned.erase(cleaned.begin());
		while (cleaned.size() && isspace(cleaned.back())) cleaned.pop_back();
		return cleaned;
	}
};
