#pragma once

#include <string>
#include <vector>

#include "token.h"

struct AST_Type {
};

struct AST_Declaration : public AST_Type {
    inline AST_Declaration() {}
    inline AST_Declaration(int line, int column, TokenType type, std::string name, std::string value, std::string scope) :
        line(line), column(column), type(type), name(name), value(value), scope(scope) {

    }

    int line;
    int column;

    TokenType type;

    std::string name;
    std::string value;
    std::string scope;
};

// CLEANUP: This can probably just be replaced with token...
struct AST_Argument : public AST_Type {
    inline AST_Argument() {}
    inline AST_Argument(std::string name, TokenType type) :
        name(name), type(type) {

    }

    std::string name;
    TokenType type;
};

struct AST_FunctionCall : public AST_Type {
    inline AST_FunctionCall() {}
    inline AST_FunctionCall(std::string name, int line, int column, std::vector<AST_Argument> arguments) :
        name(name), line(line), column(column), arguments(arguments) {

    }

    std::string name;

    int line;
    int column;

    std::vector<AST_Argument> arguments;
};

struct AST_Function : public AST_FunctionCall {
    inline AST_Function() {}
    inline AST_Function(std::string name, int line, int column, std::vector<AST_Argument> argument, std::string scope, TokenType return_type) :
        scope(scope), return_type(return_type) {
        this->name = name;
        this->line = line;
        this->column = column;
        this->arguments = arguments;
    }

    std::string scope;
    TokenType return_type;
};

struct AST_SourceFile : public AST_Type {
    inline AST_SourceFile() {}
    inline AST_SourceFile(std::string file_name) : file_name(file_name) {
        this->total_comments = 0;
        this->total_code = 0;
        this->total_blank = 0;

        this->mainFunction = nullptr; // Check: Do we really want this to be null?
    }

    std::string file_name;
    int total_comments;
    int total_code;
    int total_blank;

    std::vector<AST_Type> contained; // TODO: What should this actually be called?
    AST_Function* mainFunction;

    inline const AST_Declaration get_decl(const std::string& name) {
        for (auto potential : this->contained) {
            // We only want declarations
            if (typeid(potential) != typeid(AST_Declaration)) continue;
            // Turn the type into a declaration we can actually check
            auto decl = static_cast<AST_Declaration*>(&potential);
            // Return the declaration if it's what we're looking for
            if (decl->name == name) return *decl;
        }
        AST_Declaration decl = { -1, -1, TokenType::COMMENT, std::string(), std::string(), std::string() };
        return decl;
    }

    inline void replace_decl(AST_Declaration& replacing, AST_Declaration& new_decl) {
        int pos = -1;
        for (unsigned int i = 0; i < this->contained.size(); ++i) {
            auto potential = this->contained[i];
            // Make sure it's actually a declaration
            if (typeid(potential) != typeid(AST_Declaration)) continue;
            auto decl = static_cast<AST_Declaration*>(&potential);
            // Check if it's what we're looking for
            if (decl->name == replacing.name) {
                pos = i;
                break;
            }
        }
        // Replace the decl with our new one, if we found the old one
        if (pos > -1) this->contained[pos] = new_decl;
    }
};
