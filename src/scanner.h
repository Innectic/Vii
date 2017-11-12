#pragma once

#include <string>
#include <map>
#include <iostream>
#include <cassert>

#include "util.h"
#include "token.h"
#include "ast.h"
#include "workspace.h"
#include "reporter.h"

class Scanner {
private:
    std::string::iterator beg;
    std::string::iterator it;
    std::string::iterator end;

    std::string fileName;

    WorkSpace& workspace;

    std::map<std::string, std::map<std::string, AST_Function*>> scope_map;
    
    bool allow_native;

    AST_Math* do_math(std::vector<Token>* tokens, std::vector<Token>::iterator it, const Token& token);
public:
    std::vector<std::string> usedNames;
    Scanner(WorkSpace& workspace);
    ~Scanner();

    const Token read_string(const char& delim);
    const Token read_number();
    const bool check_comment();

    const bool can_use_name(std::string name);

    const std::vector<Token> tokenize(const std::string& filename, const bool allow_native);
    const AST_SourceFile* parse(std::vector<Token>& tokens);

    // TODO: #TooManyOverloads - This can probably just be templated, tbh

    const inline bool has_next() {
        return this->has_next(this->it, this->end);
    }

    const inline bool has_next(const std::string::iterator& it) {
        return this->has_next(it, this->end);
    }

    const inline bool has_next(const std::string::iterator& it, const std::string::iterator& end) {
        return it < end - 1;
    }

    const inline bool has_next(const std::vector<Token>::iterator& it, const std::vector<Token>::iterator& end) {
        return it < end - 1;
    }

    const inline void set_scope(const std::string& scope, AST_Function* function) {
        // Create the name of the scope based off the name of the function, and the amount of things using that name
        // that we already have stored. This should result in something like: testing$0
        auto current_scope = this->scope_map[function->name];
        auto function_scope_name = function->name + "$" + std::to_string(current_scope.size());
        // Set the function's containment scope
        function->contained_scope = function_scope_name;

        auto scope_parts = Util::split(function->contained_scope, '$');
        if (scope_parts.size() < 2) return;
        auto id = scope_parts[1];
        
        this->scope_map[function->name][id] = function;
    }

    const inline void add_scoped(std::string scope, AST_Type* type) {
        auto scope_parts = Util::split(scope, '$');
        if (scope_parts.size() < 2) return;
        auto base_scope = scope_parts[1];
        auto sub_id = scope_parts[2];

        auto scoped_function = this->scope_map[base_scope][sub_id];
        scoped_function->contained.emplace_back(type);
    }

    const inline bool next(int amount) {
        // Revisit: Speed things
        auto clone = it;
        auto had_all = true;

        for (auto i = 0; i < amount; ++i) {
            if (clone != this->end) clone++;
            else {
                had_all = false;
                break;
            }
        }
        return had_all;
    }
};
