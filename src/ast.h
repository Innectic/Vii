#pragma once

#include <string>
#include <vector>

#include "token.h"

struct AST_Type {
    virtual std::string my_name() = 0;
};

template<class First>
const inline static bool is_type(AST_Type* t) {
    return dynamic_cast<First>(t) != nullptr;
}

struct AST_Declaration : public AST_Type {
    inline AST_Declaration(const int& line, const int& column, const TokenType& type, const std::string& name, const std::string& value, const std::string& scope) :
        line(line), column(column), type(type), name(name), value(value), scope(scope) {

    }

    std::string my_name() {
        return "AST_Decl";
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
    inline AST_Argument(std::string value, TokenType type) :
        value(value), type(type) {

    }

    std::string my_name() {
        return "AST_Arg";
    }

    std::string value;
    TokenType type;
};

struct AST_FunctionCall : public AST_Type {
protected:
    inline AST_FunctionCall() {}
public:
    inline AST_FunctionCall(const std::string& name, const int& line, const int& column, const std::vector<AST_Argument>& arguments) :
        name(name), line(line), column(column), arguments(arguments) {
        this->native = false;
    }

    std::string my_name() {
        return "AST_FuncCall";
    }

    std::string name;

    int line;
    int column;

    std::vector<AST_Argument> arguments;

    bool native;
};

struct AST_Builtin : public AST_FunctionCall {
    inline AST_Builtin(const NativeType& type, const std::vector<AST_Argument>& arguments) {
        this->arguments = arguments;
        this->native = false;
        this->type = type;
    }


    std::string my_name() {
        return "AST_Builtin";
    }

    NativeType type;
    std::vector<AST_Argument> arguments;
};

struct AST_Function : public AST_FunctionCall {
    inline AST_Function(const std::string& name, const int& line, const int& column, const std::vector<AST_Argument>& arguments, std::string scope, const TokenType& return_type) :
        scope(scope), return_type(return_type) {
        this->name = name;
        this->line = line;
        this->column = column;
        this->arguments = arguments;
        this->native = false;
    }

    std::string my_name() {
        return "AST_Func";
    }

    std::string scope;
    TokenType return_type;
    std::vector<AST_Type*> contained;
};

struct AST_Keyword : public AST_Type {
    inline AST_Keyword(const KeywordType& keyword, const int& line, const int &column) :
        keyword(keyword), line(line), column(column) {

    }

    KeywordType keyword;
    int line;
    int column;
};

struct AST_SourceFile : public AST_Type {
    inline AST_SourceFile() {}
    inline AST_SourceFile(const std::string& file_name) : file_name(file_name) {
        this->total_comments = 0;
        this->total_code = 0;
        this->total_blank = 0;

        this->mainFunction = nullptr;
    }

    std::string my_name() {
        return "AST_SourceFile";
    }

    std::string file_name;
    int total_comments;
    int total_code;
    int total_blank;

    std::vector<AST_Type*> contained;
    AST_Function* mainFunction;

    inline AST_Declaration* get_decl(const std::string& name) {
        for (auto& potential : this->contained) {
            // We only want declarations
            if (!is_type<AST_Declaration*>(potential)) continue;
            // Turn the type into a declaration we can actually check
            auto decl = static_cast<AST_Declaration*>(potential);
            // Return the declaration if it's what we're looking for
            if (decl->name == name) return decl;
        }
        return nullptr;
    }

    inline void replace_decl(AST_Declaration& replacing, AST_Declaration* new_decl) {
        int pos = -1;
        for (auto i = 0u; i < this->contained.size(); ++i) {
            auto potential = this->contained[i];
            // Make sure it's actually a declaration
            if (!is_type<AST_Declaration*>(potential)) continue;
            auto decl = static_cast<AST_Declaration*>(potential);
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
