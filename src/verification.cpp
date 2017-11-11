
#include "verification.h"

const bool Verification::validate(const std::vector<AST_Type*>& contained) {
    for (auto component : contained) {
        if (is_type<AST_Function*>(component)) {
            auto function = static_cast<AST_Function*>(component);
            // Make sure something like this doesn't already exist
            auto resolved_function_type = new AST_Resolved_Function(function->name, function->line, function->column, function->arguments);
            auto registered = this->workspace.resolver.register_function(resolved_function_type);
            if (!registered) {
                this->workspace.report_error({ "=~=~=~=~=~=~ THIS IS A SPOOKY ERROR =~=~=~=~=~=~", "[file]", function->line, function->column });
                continue;
            }
        } else if (is_type<AST_FunctionCall*>(component)) {
        }
    }
    return true;
}
