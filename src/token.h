#pragma once

#include <map>
#include "util.h"

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
    { TokenType::SEMICOLON, ";" }
};

static std::map<std::string, TokenType> type_map = {
    { "string", TokenType::STRING },
    { "int", TokenType::INT },
    { "float", TokenType::FLOAT },
    { "char", TokenType::CHAR }
};

static TokenType get_token_type(std::string checking) {
    // Check each pair and see if we have a keyword that matches.
    for (auto entry : token_map) if (entry.second == checking) return entry.first;
    // If not, it's just an identifier.
    return TokenType::IDENTIFIER;
}

static TokenType get_type_from_string(std::string checking) {
    for (auto entry : type_map) if (entry.first == Util::string_to_lower(checking)) return entry.second;
    return TokenType::INVALID;
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

struct Decleration {
    int line;
    int column;
    TokenType type; // TODO: Having the type here is probably redundant.

    std::string name;
    std::string value; // TODO: Might want to turn this into some-sort of a custom type;
    std::string scope;
};

struct FunctionCall {
    int line;
    int column;

    std::string function_name;
    std::vector<Token> arguments;
};

struct Function : public FunctionCall {
    // TODO: This will only allow for primitives to be returned, custom objects won't work here.
    TokenType return_type;

    std::string parent_scope;
    std::string scope;
    bool isMain;
};

struct SourceFile {
    std::string fileName;
    int lines;
    std::vector<Function> functions;
    std::vector<FunctionCall> function_calls;
    std::vector<Decleration> decls;

    inline const Decleration getDecl(std::string name) {
        for (auto decl : this->decls) if (decl.name == name) return decl;
        return {-1, -1, TokenType::COMMENT, nullptr, nullptr, nullptr};
    }

    inline void replaceDecl(Decleration& replacing, Decleration& newDecl) {
        int pos = -1;
        for (unsigned int i = 0; i < this->decls.size(); ++i) {
            auto decl = this->decls[i];

            if (decl.name == replacing.name) {
                pos = i;
                break;
            }
        }

        if (pos > -1) this->decls[pos] = newDecl;
    }
};


