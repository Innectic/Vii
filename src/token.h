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
