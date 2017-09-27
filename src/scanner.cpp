
#include "scanner.h"

Scanner::Scanner() {

}

Scanner::~Scanner() {

}

std::vector<Token> Scanner::lexFile(const std::string& fileName) {
	std::vector<std::string> contents = Util::readFileToVector(fileName);

	std::vector<Token> tokens;

	std::string token;
	std::string str;
	bool gettingString = false;

	for (auto line : contents) {
		for (auto ch : line) {
			token += ch;
			if (token == " ") {
				if (!gettingString) token = "";
				continue;
			}
			else if (token == "print") {
				Token t = { TokenType::PRINT, nullptr };
				tokens.emplace_back(t);
				token = "";
				continue;
			}
			else if (token == "\"") {
				if (!gettingString) {
					gettingString = true;
					continue;
				}
				else if (gettingString) {
					Token token = { TokenType::STRING, str };
					tokens.emplace_back(token);
					str = "";
					gettingString = false;
					continue;
				}
			}
			else if (gettingString) {
				str += token;
				token = "";
				continue;
			}
		}
	}
	return tokens;
}
