
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
		column = 0;

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
			else if (ch == ' ') {
				if (pullingValue) current += ch;
			}
			else if (!pullingIdent && !pullingValue) {
				std::string string_ch;
				string_ch += ch;
				TokenType type = getTokenType(string_ch);
				if (type != TokenType::IDENTIFIER /*&& !pullingValue && !pullingIdent*/) {
					Token token = { type, current };
					current = "";
					tokens.emplace_back(token);
				}
			}
			else {
				if (pullingIdent || pullingValue) current += ch;
			}
    	}
	}
	return tokens;
}
