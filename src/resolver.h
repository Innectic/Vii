
#pragma once

#include <vector>
#include <string>
#include <map>

#include "ast.h"
#include "typer.h"
#include "util.h"
#include "reporter.h"

class Resolver {
private:
    std::map<std::string, std::vector<AST_Resolved_Function*>> registered_functions;
	std::map<std::string, AST_Resolved_Variable*> registered_variables;

    Typer typer;
	Reporter reporter;
public:
    inline Resolver(const Typer& typer, const Reporter& reporter) : typer(typer) {}
    
    const bool register_function(AST_Resolved_Function* type);
    AST_Resolved_Function* attempt_function_resolution(const std::vector<AST_Argument>& arguments, const std::string& name);

	const bool register_variable(AST_Resolved_Variable* variable);
	AST_Resolved_Variable* attempt_variable_resolution(const std::string& name, const std::string& scope);
};
