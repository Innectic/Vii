
#include <map>

enum class TokenType {
	INVALID,
	END_OF_FILE,
	COMMENT,
	COMMA,
	SEMICOLON,

	// Normal things
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_AND,
	OP_OR,
	LBRACE,
	RBRACE,
	LPAREN,
	RPAREN,

	IDENTIFIER,
	INT,
	FLOAT,
	CHAR,
	STRING,
	NEQ,
	EQ,
	LEQ,
	GEQ,
	COLON,
	ASSIGN,
};

// TODO: Operator precedence (AKA: Making order of operations actually apply.)
static std::map<TokenType, std::string> tokenMap = {
	{ TokenType::INVALID, "INVALID" },
	{ TokenType::END_OF_FILE, "END_OF_FILE" },
	{ TokenType::COMMENT, "COMMENT" },
	{ TokenType::OP_ADD, "+" },
	{ TokenType::OP_SUB, "-" },
	{ TokenType::OP_MUL, "*" },
	{ TokenType::OP_DIV, "/" },
	{ TokenType::OP_AND, "&&" }, // XXX: Subject to change. I HATE this operator.
	{ TokenType::OP_OR, "||" },  // XXX: Subject to change. I HATE this operator.
	{ TokenType::LBRACE, "{" },
	{ TokenType::RBRACE, "}" },
	{ TokenType::LPAREN, "(" },
	{ TokenType::RPAREN, ")" },
	{ TokenType::IDENTIFIER, "IDENTIFIER" },
	{ TokenType::INT, "INT" },
	{ TokenType::FLOAT, "FLOAT" },
	{ TokenType::CHAR, "CHAR" },
	{ TokenType::STRING, "STRING" },
	{ TokenType::NEQ, "!=" },  // XXX: Subject to change. I HATE this operator.
	{ TokenType::EQ, "==" },   // XXX: Subject to change. I MILDLY DISLIKE this operator.
	{ TokenType::LEQ, "<=" },
	{ TokenType::GEQ, ">=" },
	{ TokenType::COLON, ":" },
	{ TokenType::ASSIGN, "=" },
	{ TokenType::COMMA, "," },
	{ TokenType::SEMICOLON, ";" }
};

static TokenType getTokenType(std::string checking) {
	// Check each pair and see if we have a keyword that matches.
	for (auto entry : tokenMap) if (entry.second == checking) return entry.first;
	// If not, it's just an identifier.
	return TokenType::IDENTIFIER;
}

struct Token {
	TokenType type;
	std::string value; // TODO:  Make the type something useful
};

static std::vector<std::string> keywords = {
	"if",
	"else",
	"true",
	"false"
};
