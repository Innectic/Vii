#pragma once

#include <map>
#include <vector>
#include <string>

struct ASTProject {
public:
    std::map<std::string, std::vector<Token>> stored;
};

struct ASTType {
public:
    virtual std::string to_string() = 0;
};

struct ASTDeclaration : public ASTType {
private:
    Token token;
public:
    inline ASTDeclaration(Token token) : token(token) {}

    inline std::string to_string() {
        return token.line + "(" + std::to_string(token.column) + "): " + std::to_string((int) token.type) + std::to_string((int) token.flag);
    }
};
