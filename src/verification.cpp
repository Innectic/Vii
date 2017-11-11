
#include "verification.h"

const bool Verification::validate(const std::vector<AST_Type*>& contained) {
    for (auto component : contained) {
        if (is_type<AST_Function*>(component)) {
            auto function = static_cast<AST_Function*>(component);
            std::cout << function->name << std::endl;
            std::string argument_string;
            for (auto arg : function->arguments) argument_string += token_map[arg.type] + " ";
            std::cout << "Func args: " << argument_string << std::endl;
            if (function->contained.size() > 0) this->validate(function->contained);
            // Make sure something like this doesn't already exist
            auto resolved_function_type = new AST_Resolved_Function(function->name, function->line, function->column, function->arguments, function->scope);
            auto registered = this->workspace.resolver.register_function(resolved_function_type);
            if (!registered) {
                this->workspace.report_error({ "=~=~=~=~=~=~ THIS IS A SPOOKY ERROR =~=~=~=~=~=~", "[file]", function->line, function->column });
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
                continue;
            }
            std::string argument_string;
            for (auto arg : function_call->arguments) argument_string += token_map[arg.type] + " ";
            std::cout << "Woah, " << function_call->name << " with args " + argument_string + "was actually resolved" << std::endl;
        }
    }
    return true;
}
