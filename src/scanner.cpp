
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
            tokens.emplace_back(Token(token_type, ReservedTokenType::NONE, Flag::SCANNED, line, column));
            column++;
        }
        line++;
    }
    input.close();
    return tokens;
}

std::vector<Token> pull_tokens_until_delim(const std::vector<Token>& tokens, TokenType type) {
    std::vector<Token> found;
    For (tokens) {
        if (it.type == type) break;
        found.emplace_back(it);
    }
    return found;
}

std::vector<ASTType*> Scanner::parse(const std::vector<Token>& tokens, bool within_another_scope) {
    std::vector<ASTType*> result;

    for (auto i = tokens.begin(); i < tokens.end(); i++) {
        auto it = *i;
        auto next = i + 1;

        if (it.type == TokenType::IDENTIFIER) {
            // If this is an identifier, then we may be able to guess that this is an assigning statement
            // depending on what we find next.
            //
            // If this is an assignment, then we need to ensure we know what kind this is:
            //
            //  - :=
            //      Means we are creating a new variable with an unknown type,
            //      however the type could be provided by prefixing the `=` with the type.
            //      IE: a: i8 = 1;
            //  - =
            //      Means we are reassigning an existing variable within our current scope.
            //      The type of the provided value to reassign with should be of the same
            //      type that the already existing one is.
            //
            // We have made it this far: test := "a";
            //                           ^^^^
            //

            // Start by checking what kind of assignment this is by checking the next iterator type.
            if (next->type == TokenType::COLON) {
                // Since the next type is a colon, than this very well might be a new assignment.
                // Only way to be certain is to check if the next token a `=`?
                //   * Yes: Then we are 100% assigning a variable.
                //   * No : We still could be, but we need to ensure that this is an identifier
                //            that could be used as a type, and that the following token is
                //            a `=`.
                //
                // We have made it this far: test := "a";
                //                           ^^^^ ^-  We're also kind of at the `=` here, depending on what we find.
                auto typeStyle = TypeOfType::UNKNOWN;
                
                auto the_next = i + 2;
                if (the_next->type == TokenType::EQUALS) typeStyle = TypeOfType::INFERRED;
                else if (the_next->type == TokenType::IDENTIFIER) typeStyle = TypeOfType::GIVEN;
                // Now that we know how the variable will be assigned, we can actually make the
                // assignment meta information.
                auto to = *(typeStyle == TypeOfType::INFERRED ? i + 3 : i + 4);
                ASTType* assignment = new ASTAssignment(it, to, typeStyle);
                result.emplace_back(assignment);
            }
        } else if (it.type == TokenType::RESERVED) {
            // If this is a reserved type, then this is a language keyword.
            if (it.reserved == ReservedTokenType::STRUCT) {
                // First, we need to make sure we're not trying to make a struct in a struct.
                if (within_another_scope) {
                    // TODO: Put an error here.
                    return result;
                }
                // Since we're not trying to put a struct in a struct, we can move on to parsing
                // the components of the struct.
                auto struct_tokens = pull_tokens_until_delim(tokens, TokenType::LBRACE);
                // Now that we have the tokens up until (hopefully) the end of the struct
                // declaration, we can send this through the parser and figure out what
                // we have in here.
                auto parse_result = this->parse(struct_tokens, true);
            }
        }
    }
    return result;
}
