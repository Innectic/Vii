#pragma once

#include <string>
#include <iostream>
#include <map>

#include "util.h"
#include "token.cpp"

class Scanner {
public:
	Scanner();
	~Scanner();

	std::vector<Token> lexFile(const std::string& filename);
};
