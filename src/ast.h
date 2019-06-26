#pragma once

#include <map>
#include <vector>
#include <string>

struct ASTType {
public:
    virtual std::string to_string() = 0;
};

enum class TypeOfType {
    UNKNOWN,
    INFERRED,
    GIVEN
};

struct ASTProject {
public:
    std::vector<ASTType*> types;

    inline void store_ast_segment(std::string file, ASTType* segment) {
        this->types.emplace_back(segment);
    }
};

struct ASTAssignment : public ASTType {
private:
    Token& assigning;
    Token& to;
    TypeOfType typeStyle;
public:
    inline ASTAssignment(Token& assigning, Token& to, TypeOfType type) : assigning(assigning), to(to), typeStyle(type) {}

    inline std::string to_string() {
        return "WE ARE (RE)ASSIGNING A VARIABLE WEE";
    }
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

struct ASTStruct : public ASTType {
 private:
    
};
