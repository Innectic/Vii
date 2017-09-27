#pragma once

#include <regex>
#include <string>
#include <vector>
#include <fstream>

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
};
