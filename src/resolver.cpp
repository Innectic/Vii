
#include "resolver.h"

const bool Resolver::register_function(AST_Resolved_Function* type) {
	assert(type);

    auto resolved = this->attempt_function_resolution(type->arguments, type->name);
    if (resolved) return false;
        
    auto types = registered_functions[type->name];
    types.emplace_back(type);
	registered_functions[type->name] = types;
    return true;
}

AST_Resolved_Function* Resolver::attempt_function_resolution(const std::vector<AST_Argument>& arguments, const std::string& name) {
    if (name == "") return nullptr;
    if (!Util::contains_key(registered_functions, name)) return nullptr;

    auto resolved_types = registered_functions[name];
	for (auto resolved_it = resolved_types.begin(); resolved_it < resolved_types.end(); resolved_it++) {
		auto resolved = *resolved_it;
		bool all_have_matched_so_far = true;
		// Make sure the function's typedef can even attempt to match
		// TODO: This will break with optional / default arguments
		//
		if (resolved->arguments.size() != arguments.size()) continue;
		for (auto resolved_argument = resolved->arguments.begin(); resolved_argument < resolved->arguments.end(); resolved_argument++) {
			for (auto given = arguments.begin(); given < arguments.end(); given++) {
				// Can we assign these two types to each other?
				if (this->typer.can_assign_this(given->type, resolved_argument->type)) {
					// We can, so we're matching.
					all_have_matched_so_far = true;
					continue;
				} else if (given->type == TokenType::IDENTIFIER) {
					// We're passing in a variable. Get it, and see if it can go here.
					auto resolved_variable = this->attempt_variable_resolution(given->value, "TODO: MAKE SCOPING WORK HERE");
					// Make sure we got something back
					if (!resolved_variable) {
						// Since we don't have something, this variable doesn't exist. Let the user know. (This should probably be done at parse-time?)
						this->reporter.report_error({
							"Cannot access variable '" + given->value + "' within function call. Variable does not exist!",
							"somewhere", // TODO
							0,
							0
						});
						return nullptr; // TODO: Is this the correct response?
					}
					// We have something, so make sure it's assignable.
					if (!this->typer.can_assign_this(resolved_argument->type, resolved_variable->decl->type)) {
						// It's not, error out.
						this->reporter.report_error({
							"Cannot assign variable '" + given->value + "' to type " + token_map[resolved_argument->type] + " within function call.",
							"somewhere", // TODO
							resolved_variable->line,
							resolved_variable->column
						});
						return nullptr;
					}
					// Success!
					all_have_matched_so_far = true;
				} else {
					// Invalid type
					this->reporter.report_error({
						"Cannot assign variable '" + given->value + "' to type " + token_map[resolved_argument->type] + " within function call.",
						"somewhere", // TODO
						0,
						0
					});
					return nullptr;
				}
			}
		}
		if (all_have_matched_so_far) return resolved;
	}
	return nullptr;
}

const bool Resolver::register_variable(AST_Resolved_Variable* variable) {
	assert(variable);

	// Make sure this hasn't been resolved yet.
	auto resolved = this->attempt_variable_resolution(variable->name, variable->scope);
	if (resolved) return false;
	
	// It hasn't, resolve it ourselves.
	this->registered_variables[variable->name] = variable;
	return true;
}

AST_Resolved_Variable* Resolver::attempt_variable_resolution(const std::string& name, const std::string& scope) {
	if (!Util::contains_key(this->registered_variables, name)) return nullptr;

	auto registered = this->registered_variables[name];
	assert(registered);
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	// TODO: Check scoping.
	return registered;
}
