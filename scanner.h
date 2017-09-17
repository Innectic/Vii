#pragma once

#include "token.cpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

class Scanner {
private:
	std::vector<Token> tokenized;
public:
	std::vector<Token> scan(const std::string& fileName);
	std::vector<Token> takeUntilWhitespace(const std::string& reading);
};
