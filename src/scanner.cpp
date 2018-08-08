
#include "scanner.h"

Scanner::Scanner() {

}

Scanner::~Scanner() {

}

std::vector<Token> Scanner::tokenize(const std::string& file) {
    std::vector<Token> tokens;

    std::ifstream input;
    input.open(file);

    if (!input) return tokens;
    std::string current = "";
    while (std::getline(input, current)) {
        For (current) {
            // Skip white-space
            if (it == ' ') continue;

            // Get the type of the token. If it isn't a "special" token type, like `/`, then it will just be `IDENTIFIER`
            auto token_type = get_token_type(it);
            tokens.emplace_back(Token(token_type, Flag::SCANNED));
        }
    }
    input.close();
    return tokens;
}

void Scanner::parse() {

}
