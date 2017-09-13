#pragma once

#include <regex>
#include <string>

class Util {
public:
	static inline std::string eatSpaces(std::string original) {
		return std::regex_replace(original, std::regex("^ +| +$|( ) +"), "$1");
	}
};
