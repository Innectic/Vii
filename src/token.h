
enum class Flag {
    SCANNED,
    PARSED,
    OPTIMIZED_AWAY,
    OPTIMIZED_KEPT,
    EXPORTED
};

enum class TokenType {
    END_OF_FILE,
    INVALID,

    FORWARD_SLASH,
    BACKWARD_SLASH,
    IDENTIFIER,

    PLUS,
    MINUS,
    ASTERISK,
    EQUALS,
    SEMICOLON,
    COLON,
    PERIOD,

    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    LPAREN,
    RPAREN,

    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    RESERVED
};

enum class ReservedTokenType {
    NONE,
    STRUCT
};

struct Token {
public:
    ReservedTokenType reserved;
    TokenType type;
    Flag flag;

    int column;
    int line;

Token(TokenType type, ReservedTokenType reserved, Flag flag, int line, int column) : type(type), flag(flag), line(line), column(column), reserved(reserved) {}
};

const static TokenType get_token_type(const char& ch) {
    switch (ch) {
        case '/': return TokenType::FORWARD_SLASH;
        case '\\': return TokenType::BACKWARD_SLASH;
        case '+': return TokenType::PLUS;
        case '-': return TokenType::MINUS;
        case '*': return TokenType::ASTERISK;
        case '=': return TokenType::EQUALS;
        case ';': return TokenType::SEMICOLON;
        case ':': return TokenType::COLON;
        case '.': return TokenType::PERIOD;
        case '{': return TokenType::LBRACE;
        case '}': return TokenType::RBRACE;
        case '[': return TokenType::LBRACKET;
        case ']': return TokenType::RBRACKET;
        case '(': return TokenType::LPAREN;
        case ')': return TokenType::RPAREN;
        case '\'': return TokenType::SINGLE_QUOTE;
        case '"': return TokenType::DOUBLE_QUOTE;
        default: return TokenType::IDENTIFIER;
    }
}
