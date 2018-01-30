#pragma once

#include <string>
#include <vector>

class ViiString {
private:
public:
	std::string str;

	ViiString(const std::string& str);
	ViiString(const char* str);

	bool contains(const ViiString& contains);
	std::vector<ViiString> split(const ViiString& by, const int limit = 0);
};
