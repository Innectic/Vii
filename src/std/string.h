#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

class ViiString {
private:
public:
	std::string str;

	ViiString(const std::string& str);
	ViiString(const char* str);

	bool contains(const ViiString& contains);
	std::vector<ViiString> split(const char& by, const int limit = 0);

	bool starts_with(const ViiString& checking);
	bool ends_with(const ViiString& checking);
};
