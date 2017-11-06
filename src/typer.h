#pragma once

#include "token.h"
#include "ast.h"

class Typer {
public:
    const inline bool can_assign_this(const TokenType& first, const TokenType& second) const {
        if (first == second) return true;
        else if (first == TokenType::INT && second == TokenType::FLOAT) return true;
        else if (second == TokenType::INT && first == TokenType::FLOAT) return true;
        return false;
    }

    const inline bool function_take_arguments(const AST_Function& function, std::vector<TokenType> arguments) {
        // Check size first
        if (function.arguments.size() != arguments.size()) return false; // TODO: This will break with the implementation of defaults & optional params
        for (auto& arg : function.arguments) {
            for (auto& other_arg : function.arguments) {
                if (!can_assign_this(arg.type, other_arg.type)) return false;
            }
        }
        // If all types match, then we're good.
        return true;
    }
};
