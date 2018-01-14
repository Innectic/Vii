
#include "verification.h"

const bool Verification::validate(const std::vector<AST_Type*>& contained) {
    auto had_issues = false;
    for (auto component : contained) {
        if (is_type<AST_Function*>(component)) {
            auto function = static_cast<AST_Function*>(component);
            if (function->contained.size() > 0) this->validate(function->contained);
            // Make sure something like this doesn't already exist
            auto resolved_function_type = new AST_Resolved_Function(function->name, function->line, function->column, function->arguments, function->scope);
            auto registered = this->workspace.resolver.register_function(resolved_function_type);
            if (!registered) {
                std::string argument_string;
                for (auto arg : function->arguments) argument_string += token_map[arg.type] + " ";
                this->workspace.report_error({ "Overloaded function " + function->name + " with arguments " + argument_string + "already exists.", "[file]", function->line, function->column });
                had_issues = true;
                continue;
            }
        } else if (is_type<AST_FunctionCall*>(component) && !is_type<AST_Builtin*>(component)) {
            auto function_call = static_cast<AST_FunctionCall*>(component);
            if (function_call->native) {
                // TODO: Make this work
                continue;
            }
            auto resolved_function = this->workspace.resolver.attempt_function_resolution(function_call->arguments, function_call->name);
            if (!resolved_function) {
                // Function with these arguments doesn't exist.
                std::string argument_string;
                for (auto arg : function_call->arguments) argument_string += token_map[arg.type] + " ";
                this->workspace.report_error({ "Function with " + argument_string + "by name " + function_call->name + " doesn't exist.", "[file]", function_call->line, function_call->column });
                had_issues = true;
                continue;
            }
		} else if (is_type<AST_Declaration*>(component)) {
			auto decl = static_cast<AST_Declaration*>(component);
			auto resolved = new AST_Resolved_Variable(decl->name, decl->line, decl->column, decl->scope, decl);
			auto registered = this->workspace.resolver.register_variable(resolved);
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
			// TODO: This breaks with redecl
		}
    }
    return had_issues;
}
