#pragma once

#include <string>
#include <vector>
#include "typer.h"

struct Symbol {
	std::string name;
	std::string value;
	Type type;

	int line;
	int column;
};

struct Function {
	int line;
	int column;
	std::string name;

	std::vector<Symbol> arguments;
};
