#pragma once

#include "token.h"
#include "ast.h"

class Typer {
public:
    const inline bool can_assign_this(const TokenType& first, const TokenType& second) const {
        // TODO: clean this up
        if (first == second) return true;
        else if (first == TokenType::INT && second == TokenType::FLOAT) return true;
        else if (second == TokenType::INT && first == TokenType::FLOAT) return true;
        return false;
    }
};
