
#pragma once

#include <vector>
#include <string>
#include <map>

#include "ast.h"
#include "typer.h"
#include "util.h"

class Resolver {
private:
    std::map<std::string, std::vector<AST_Resolved_Function*>> registered;
    Typer typer;
public:
    inline Resolver(const Typer& typer) : typer(typer) {}
    
    const bool register_function(AST_Resolved_Function* type);
    AST_Resolved_Function* attempt_function_resolution(const std::vector<AST_Argument>& arguments, const std::string& name);
};
