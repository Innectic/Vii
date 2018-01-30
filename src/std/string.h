#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

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

	ViiString trim();
	ViiString replace(ViiString replacing, ViiString with);

	ViiString lower();
	ViiString upper();

	inline bool operator==(const ViiString& other) {
		return this->str == other.str;
	}

	inline bool operator==(const std::string& other) {
		return this->str == other;
	}

	inline bool operator!=(const ViiString& other) {
		return this->str != other.str;
	}

	inline bool operator!=(const std::string& other) {
		return this->str != other;
	}

	inline ViiString operator+=(const std::string adding) {
		this->str += adding;
		return *this;
	}

	inline ViiString operator+=(const ViiString& adding) {
		this->str += adding.str;
		return *this;
	}
	
	inline ViiString operator=(const ViiString& value) {
		this->str = value.str;
		return *this;
	}
};

static std::ostream& operator<<(std::ostream& stream, const ViiString& other) {
	stream << other.str;
	return stream;
}
