#pragma once

#include <string>
#include <vector>
#include "symbol.h"

// SourceFile contains all the parsed information about a file.
struct SourceFile {
	std::string fileName;
	int lines;
	std::vector<Symbol> symbols;
	std::vector<Function> functions;
};
