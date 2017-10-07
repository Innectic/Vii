#pragma once

#include <string>
#include <map>
#include <iostream>

#include "util.h"
#include "token.cpp"

class Scanner {
private:
	std::string::iterator beg;
	std::string::iterator it;
	std::string::iterator end;
public:
	Scanner();
	~Scanner();

	const std::string read_string();
	const std::string read_number();

	const std::vector<Token> lex_file(const std::string& filename);

	const inline bool hasNext() const {
		return this->it < end;
	}
};
