
#include "scanner.h"

Scanner::Scanner() {

}

Scanner::~Scanner() {

}

std::vector<Token> Scanner::tokenize(const std::string& file) {
    std::vector<Token> tokens;

    std::ifstream input;
    input.open(file);

    int line = 0;

    if (!input) return tokens;
    std::string current = "";
    while (std::getline(input, current)) {
        int column = 0;

        For (current) {
            // Skip white-space
            if (it == ' ') {
                column++;
                continue;
            }

            // Get the type of the token. If it isn't a "special" token type, like `/`, then it will just be `IDENTIFIER`
            auto token_type = get_token_type(it);
            tokens.emplace_back(Token(token_type, Flag::SCANNED, line, column));
            column++;
        }
        line++;
    }
    input.close();
    return tokens;
}

ASTProject* Scanner::parse(const std::vector<Token>& tokens) {
    auto project = new ASTProject();

    for (auto i = tokens.begin(); i < tokens.end(); i++) {
        auto it = *i;
        auto next = i + 1;

        if (it.type == TokenType::IDENTIFIER) {

        }
    }

    return project;
}
