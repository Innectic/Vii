#pragma once

#include <map>

enum class Token {
	INVALID,
	END_OF_FILE,
	COMMENT,

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
	DEFINE,

	// Keywords
	PRINT
};

// TODO: Operator precedence (AKA: Making order of operations actually apply.)
static std::map<Token, std::string> tokenMap = {
	{Token::INVALID, "INVALID"},
	{Token::END_OF_FILE, "END_OF_FILE"},
	{Token::IDENTIFIER, "IDENTIFIER"},
	{Token::COMMENT, "COMMENT"},
	{Token::OP_ADD, "+"},
	{Token::OP_SUB, "-"},
	{Token::OP_MUL, "*"},
	{Token::OP_DIV, "/"},
	{Token::OP_AND, "&&"}, // XXX: Subject to change. I HATE this operator.
	{Token::OP_OR, "||"},  // XXX: Subject to change. I HATE this operator.
	{Token::LBRACE, "{"},
	{Token::RBRACE, "}"},
	{Token::LPAREN, "("},
	{Token::RPAREN, ")"},
	{Token::IDENTIFIER, "IDENTIFIER"},
	{Token::INT, "INT"},
	{Token::FLOAT, "FLOAT"},
	{Token::CHAR, "CHAR"},
	{Token::STRING, "STRING"},
	{Token::NEQ, "!="},  // XXX: Subject to change. I HATE this operator.
	{Token::EQ, "=="},   // XXX: Subject to change. I MILDLY DISLIKE this operator.
	{Token::LEQ, "<="},
	{Token::GEQ, ">="},
	{Token::DEFINE, ":="},
	{Token::PRINT, "print"}
};

static Token getToken(std::string checking) {
	// Check each pair and see if we have a keyword that matches.
	for (auto entry : tokenMap) if (entry.second == checking) return entry.first;
	// If not, it's just an identifier.
	return Token::IDENTIFIER;
}
