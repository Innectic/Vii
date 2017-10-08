#pragma once

#include <string>
#include <iostream>
#include "util.h"

enum Type {
	INT, STRING, NONE, UNKNOWN
};

class Typer {
	// TODO: Make this a real thing, and not a massive joke

public:
	static inline const Type get_type(const std::string value) {
		if (Util::is_number(value)) return Type::INT;
		return Type::STRING;
	}

	static inline const Type get_type_by_name(const std::string name) {
		if (name == "int") return Type::INT;
		else if (name == "string") return Type::STRING;
		return Type::UNKNOWN;
	}

};
