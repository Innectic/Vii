#pragma once

#include "token.h"
#include "ast.h"

class Typer {
public:
    const inline bool can_assign_this(const TokenType& first, const TokenType& second) const {
        if (first == second) return true;
        else if (is_numeric(first) && is_numeric(second)) return true;
        return false;
    }
};
