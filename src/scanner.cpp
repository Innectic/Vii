
#include "scanner.h"
#include <chrono>

Scanner::Scanner() {

}

Scanner::~Scanner() {

}

const std::string Scanner::read_string() {
	bool escaped = false;
	bool taking = false;
	std::string found = "";

	while (it < this->end) {
		if (escaped) {
			found += *it;
			escaped = false;
		}
		else if (*it == '\\' && this->hasNext() && *(it + 1) == '"') escaped = true;
		else if (*it == '"') {
			if (!escaped && taking) taking = false;
			else taking = true;
		}
		else if (taking) {
			found += *it;
		}
		it++;
	}

	return found;
}

// TODO: Make this actually do something useful...
// This only supports things like 123, nothing like 12.3
//
const std::string Scanner::read_number() {
	std::string found = "";
	while (it < this->end) {
		if (!Util::isNumber(*it)) break;
		found += *it;
		it++;
	}
	return found;
}

const std::vector<Token> Scanner::lex_file(const std::string& fileName) {
	auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	std::vector<std::string> contents = Util::readFileToVector(fileName);
	std::vector<Token> tokens;
	if (contents.size() <= 0) return tokens;

	for (auto line : contents) {
		this->beg = line.begin();
		this->it = this->beg;
		this->end = line.end();

		std::string current = "";

		while (it < end) {
			if (*it == '"') {
				auto taken = this->read_string();
				Token token = { TokenType::STRING, taken };
				tokens.emplace_back(token);
				continue;
			} else if (*it == '/' && this->hasNext() && *(this->it + 1) == '/') {
				it += 2;
				continue;
			}
			else if (Util::isNumber(*it)) {
				auto taken = this->read_number();
				Token token = { TokenType::INT, taken };
				tokens.emplace_back(token);
				continue;
			}
			else {
				std::string a;
				a += *it;

				// std::cout << a << std::endl;

				auto type = getTokenType(a);
				if (type == TokenType::IDENTIFIER) {
					current += *it;
					it++;
					continue;
				}

				if (current != "" && current != " ") {
					Token identToken = { TokenType::IDENTIFIER, current };
					tokens.emplace_back(identToken);
					current = "";
				}

				Token token = { type, "" };
				tokens.emplace_back(token);
			}
			it++;
			continue;
		}
	}
	auto nowTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	std::cout << "\033[1;36m> \033[0;32mFrontend time (s): " <<  ((double) (nowTime - time) / 1000000000) << "\033[0m\n" << std::endl;
	return tokens;
}
