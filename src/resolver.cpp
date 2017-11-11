
#include "resolver.h"

const bool Resolver::register_function(AST_Resolved_Function* type) {
    if (!type) return false;

    auto resolved = this->attempt_function_resolution(type->arguments, type->name);
    if (resolved) return false;
        
    std::vector<AST_Resolved_Function*> types;
    types.emplace_back(type);
    registered[type->name] = types;
    return true;
}

AST_Resolved_Function* Resolver::attempt_function_resolution(const std::vector<AST_Argument>& arguments, const std::string& name) {
    if (name == "") return nullptr;
    if (!Util::contains_key(registered, name)) return nullptr;

    auto resolved_types = registered[name];
    for (auto resolved : resolved_types) {
        bool did_resolve = false;
        if (resolved->arguments.size() != arguments.size()) continue;
        for (auto res_argument : resolved->arguments) {
            for (auto given_argument : arguments) {
                if (res_argument.type == given_argument.type) {
                    did_resolve = true;
                    continue;
                }
                if (did_resolve) break;
            }
            if (did_resolve) break;
        }
        if (did_resolve) return resolved;
    }
    return nullptr;
}
