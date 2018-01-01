#pragma once

#include <map>
#include "util.h"

enum class TokenType {
    INVALID,
    END_OF_FILE,
    COMMENT,
    COMMA,
    SEMICOLON,

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
    BOOL
};

enum class NativeType {
    PRINT
};

// TODO: Operator precedence (AKA: Making order of operations actually apply.)
static std::map<TokenType, std::string> token_map = {
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
    { TokenType::INVALID, "INVALID" },
    { TokenType::NEQ, "!=" },  // XXX: Subject to change. I HATE this operator.
    { TokenType::EQ, "==" },   // XXX: Subject to change. I MILDLY DISLIKE this operator.
    { TokenType::LEQ, "<=" },
    { TokenType::GEQ, ">=" },
    { TokenType::COLON, ":" },
    { TokenType::ASSIGN, "=" },
    { TokenType::COMMA, "," },
    { TokenType::SEMICOLON, ";" },
    { TokenType::BOOL, "BOOL" },
};

static std::map<std::string, TokenType> type_map = {
    { "string", TokenType::STRING },
    { "int", TokenType::INT },
    { "float", TokenType::FLOAT },
    { "char", TokenType::CHAR },
    { "bool", TokenType::BOOL },
};

static std::map<NativeType, std::string> internal_map = {
    { NativeType::PRINT, "print(<CUSTOM>);" }
};

static std::map<NativeType, std::string> native_map = {
    { NativeType::PRINT, "std::cout << <CUSTOM> << std::endl;"}
};

static std::map<std::string, NativeType> builtin_map = {
    { "print", NativeType::PRINT }
};

const static bool is_operator(const TokenType& type) {
    return type == TokenType::OP_ADD || type == TokenType::OP_SUB || type == TokenType::OP_MUL || type == TokenType::OP_DIV;
}

const static bool is_conditional_operator(const TokenType& type) {
	return type == TokenType::ASSIGN;
}

const static std::string conditional_operator_as_string(const TokenType& type) {
	// TODO: Make this not bad
	switch (type) {
		case TokenType::ASSIGN:
			return "=";
		default:
			return "invalid";
	}
}

const static bool is_builtin(const std::string& name) {
    for (auto& entry : builtin_map) if (entry.first == name) return true;
    return false;
}

static std::string get_native(const std::string& name) {
    auto using_name = Util::replace(name, "native_", "");
    return native_map[builtin_map[using_name]];
}

static TokenType get_token_type(const std::string& checking) {
    if (checking == "true" || checking == "false") return TokenType::BOOL;
    // Check each pair and see if we have a keyword that matches.
    for (auto& entry : token_map) if (entry.second == checking) return entry.first;
    // If not, it's just an identifier.
    return TokenType::IDENTIFIER;
}

static TokenType get_type_from_string(const std::string& checking) {
    if (checking == "true" || checking == "false") return TokenType::BOOL;
    for (auto& entry : type_map) if (entry.first == checking) return entry.second;
    return TokenType::INVALID;
}

const static bool is_numeric(const TokenType& type) {
    return type == TokenType::INT || type == TokenType::FLOAT;
}

struct Token {
    TokenType type;
    std::string value;

    int line;
    int column;
};

enum class KeywordType {
    INVALID,
    IF,
    ELSE,
    TRUE,
    FALSE
};

static std::map<std::string, KeywordType> keywords = {
    { "if", KeywordType::IF },
    { "else", KeywordType::ELSE },
    { "true", KeywordType::TRUE },
    { "false", KeywordType::FALSE },
};

static KeywordType get_keyword_type(std::string keyword) {
    for (auto& entry : keywords) if (entry.first == keyword) return entry.second;
    return KeywordType::INVALID;
}
