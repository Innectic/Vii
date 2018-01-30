
#include "string.h"

ViiString::ViiString(const std::string& str) : str(str) {
}

ViiString::ViiString(const char* str) {
	this->str = std::string(str);
}

bool ViiString::contains(const ViiString& contains) {
	return this->str.find(contains.str) != std::string::npos;
}

std::vector<ViiString> ViiString::split(const char& by, const int limit) {
	std::vector<ViiString> matches;

	std::string part;
	std::stringstream stream(this->str);
	while (getline(stream, part, by)) matches.emplace_back(ViiString(part));

	return matches;
}

bool ViiString::starts_with(const ViiString& checking) {
	return this->str.substr(0, checking.str.size()) == checking.str;
}

bool ViiString::ends_with(const ViiString& checking) {
	if (checking.str.length() > this->str.length()) return false;

	auto start_pos = this->str.length() - checking.str.length();
	return this->str.substr(start_pos) == checking.str;
}

ViiString ViiString::trim() {
	auto cleaned = this->str;

	while (cleaned.size() && isspace(cleaned.front())) cleaned.erase(cleaned.begin());
	while (cleaned.size() && isspace(cleaned.back())) cleaned.pop_back();

	return cleaned;
}

ViiString ViiString::replace(ViiString replacing, ViiString with) {
	auto replaced = this->str;
	replaced = std::regex_replace(replaced, std::regex(replacing.str), with.str);
	return replaced;
}

ViiString ViiString::upper() {
	auto current = this->str;
	std::transform(current.begin(), current.end(), current.begin(), ::toupper);
	return current;
}

ViiString ViiString::lower() {
	auto current = this->str;
	std::transform(current.begin(), current.end(), current.begin(), ::tolower);
	return current;
}
