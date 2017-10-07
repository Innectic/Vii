
#include "scanner.h"

Scanner::Scanner() {

}

Scanner::~Scanner() {

}

std::vector<Token> Scanner::lexFile(const std::string& fileName) {
	std::vector<std::string> contents = Util::readFileToVector(fileName);
	std::vector<Token> tokens;

	int lineNum = 0;
	int column = 0;

	bool pullingValue = false;
	bool pullingIdent = false;
	std::string current;

	for (auto line : contents) {
		lineNum++;

		for (char ch : line) {
			column++;
			if (pullingIdent) {
				Token token = { TokenType::IDENTIFIER, current };
				current = "";
				tokens.emplace_back(token);
				pullingIdent = false;
				continue;
			}

			if (ch == '\"') {
				if (pullingValue) {
					Token token = { TokenType::STRING, current };
					tokens.emplace_back(token);
					current = "";
					pullingValue = false;
					continue;
				}
				pullingValue = true;
				continue;
			}
			else if (ch == ' ' && !pullingValue) {
				continue;
			}
			else {
				std::string string_ch;
				string_ch.push_back(ch);
				TokenType type = getTokenType(string_ch); // HACK: Do this the right way
				if (type != TokenType::IDENTIFIER && !pullingValue && !pullingIdent) {
					Token token = { type, current };
					current = "";
					tokens.emplace_back(token);
				}
			}

			if (pullingValue) {
				current += ch;
				continue;
			}
    	}
	}
	return tokens;
}
