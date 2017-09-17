
#include "scanner.h"

std::vector<Token> Scanner::scan(const std::string& fileName) {
	std::ifstream fileStream(fileName);
	if (!fileStream) return this->tokenized;

	std::string current = "";
	while (std::getline(fileStream, current)) {
		std::vector<Token> tokens = takeUntilWhitespace(current);
		for (auto token : tokens) this->tokenized.push_back(token);
	}
	for (auto a : this->tokenized) std::cout << tokenMap[a] << std::endl;

	fileStream.close();
	return tokenized;
}

std::vector<Token> Scanner::takeUntilWhitespace(const std::string& reading) {
	std::string taken = "";
	bool takingValue = false;

	std::vector<Token> found;

	// TODO: This should pull from the workspace
	if (reading.substr(0, 2) == "//") return found;

	for (char ch : reading) {
		std::cout << ch << std::endl;
		if ((ch == ' ' || ch == '\n') && !takingValue) {
			// TODO: Get the type of the token. (Is it a string, int, etc)
			// TODO: Store the value
			// TODO: Symbol table things
			Token token = Token::STRING;
			found.push_back(token);
			continue;
		}
		taken += ch;
	}
	return found;
}
