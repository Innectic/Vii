#pragma once

#include <string>
#include <iostream>

enum Type {
	INT, STRING, NONE, UNKNOWN
};

class Typer {
	// TODO: Make this a real thing, and not a massive joke

public:
	static inline const Type getType(const std::string value) {
		for (char c : value) {
			if (isdigit(c)) {
				return Type::INT;
			}
		}
		return Type::STRING;
	}

	static inline const Type getTypeByName(const std::string name) {
		if (name == "int") return Type::INT;
		else if (name == "string") return Type::STRING;
		return Type::UNKNOWN;
	}

};