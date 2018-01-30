
#include "string.h"

ViiString::ViiString(const std::string& str) : str(str) {
}

ViiString::ViiString(const char* str) {
	this->str = std::string(str);
}

bool ViiString::contains(const ViiString& contains) {
	return this->str.find(contains.str) != std::string::npos;
}

std::vector<ViiString> ViiString::split(const ViiString& by, const int limit) {
	std::vector<ViiString> matches;

	return matches;
}
