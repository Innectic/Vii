
#include "scanner.h"

Scanner::Scanner(WorkSpace& workspace) : workspace(workspace) {

}

Scanner::~Scanner() {

}

const Token Scanner::read_string(const char& delim) {
    bool escaped = false;
    bool taking = false;

    bool was_terminated = true;

    std::string found = "";

    Token token = { TokenType::INVALID, "" };

    while (it <= this->end) {
        if (it == this->end && *(it - 1) != delim) {
            // This means it was never teminated...
            was_terminated = false;
            break;
        }

        if (escaped) {
            found += *it;
            escaped = false;
        }
        else if (*it == '\\' && this->has_next() && *(it + 1) == delim) escaped = true;
        else if (*it == delim) {
            if (!escaped && taking) {
                taking = false;
                it++;
                break;
            }
            else taking = true;
        }
        else if (taking) found += *it;
        it++;
    }

    if (was_terminated) {
        token.type = delim == '\'' ? TokenType::CHAR : TokenType::STRING;
        token.value = found;
    }

    return token;
}

const Token Scanner::read_number() {
    std::string found = "";
    bool encountered_dot = false;
    bool invalid = false;

    Token token = { TokenType::INVALID, "0" };

    while (it < this->end) {
        if (*it == '.') {
            if (encountered_dot) {
                invalid = true;
                break;
            }
            encountered_dot = true;
        }
        else if (!Util::is_number(*it)) break;
        found += *it;
        ++it;
    }
    if (!invalid) {
        token.value = found;
        token.type = encountered_dot ? TokenType::FLOAT : TokenType::INT;
    }
    return token;
}

const bool Scanner::check_comment() {
    // Clone the iterator
    auto i = this->it;

    bool all_have_matched = true;

    // Make sure the iterator starts with the whole comment prefix
    for (auto& c : this->workspace.comment_prefix) {
        if (!has_next(i)) return false;
        all_have_matched = *i == c;
        ++i;
    }

    return all_have_matched;
}

const bool Scanner::can_use_name(std::string name) {
    // TODO: This needs more checking for the contents of the name too.
    // We don't want names like: `t-h-e_t-h-i-n-g`. 
    return !Util::vectorContains(this->usedNames, name) && get_keyword_type(name) == KeywordType::INVALID;
}

const std::vector<Token> Scanner::tokenize(const std::string& fileName, const bool allow_native) {
    this->fileName = fileName;
    this->allow_native = allow_native;

    std::vector<std::string> contents = Util::readFileToVector(fileName);
    std::vector<Token> tokens;
    if (contents.size() <= 0) return tokens;

    bool skip_next = false;
    int line_num = 1;
    for (auto& line : contents) {
        int column = 1;
        this->beg = line.begin();
        this->it = this->beg;
        this->end = line.end();

        std::string current = "";

        while (it < end) {
            if (skip_next) {
                if (*it == '*' && this->has_next() && *(it + 1) == '/') {
                    skip_next = false;
                    it += 2;
                    continue;
                }
                else {
                    ++it;
                    continue;
                }
            }
            if (*it == '/' && this->has_next() && *(it + 1) == '*' && !skip_next) {
                skip_next = true;
                ++it;
                continue;
            }
            else if (*it == '"' || *it == '\'') {
                auto token = this->read_string(*it);
                if (token.type == TokenType::INVALID) {
                    this->workspace.report_error({ "String / char was never terminated.", this->fileName, 0, column });
                    continue;
                }
                tokens.emplace_back(token);
                continue;
            }
            else if (this->check_comment()) {
                it += 2;
                continue;
            }
            else if (Util::is_number(*it)) {
                auto token = this->read_number();
                if (token.type == TokenType::INVALID) {
                    this->workspace.report_error({ "Number cannot contain multiple '.'", this->fileName, 0, column });
                    continue;
                }
                tokens.emplace_back(token);
                continue;
            }
            else {
                std::string contents = *it;

                if (contents == " " || contents == "") {
                    it++;
                    continue;
                }

                // Get the type of the character we're currently looking at
                auto type = get_token_type(contents);

                // If it's an ident, that means it's not something we have.
                // So, add that to our currently found (for later use) string.
                if (type == TokenType::IDENTIFIER && (contents != " " && contents != "")) {
                    current += contents;
                    ++it;
                    continue;
                }

                if (current != "" && current != " ") {
                    Token identToken = { TokenType::IDENTIFIER, current, line_num, column };
                    tokens.emplace_back(identToken);
                    current = "";
                }

                Token token = { type, "", line_num, column };
                tokens.emplace_back(token);
            }
            ++it;
            ++column;
        }
        ++line_num;
    }
    return tokens;
}

const AST_SourceFile* Scanner::parse(std::vector<Token>& tokens) {
    AST_SourceFile* file = new AST_SourceFile(this->fileName);

    // Start by making sure we even have any tokens to parse.
    if (tokens.size() == 0) return file;
    std::string current_scope = "";

    // We have things to check, so lets just start going through.
    for (auto& it = tokens.begin(); it < tokens.end() - 1; ++it) {
        auto token = *it;
        // Check the type of the token
        
        if (token.type == TokenType::RBRACE) {
            current_scope = this->fileName;
            continue;
        }
        else if (token.type == TokenType::IDENTIFIER) {
            // If we have an identifier type, this can be four different things.
            //   - Function call
            //   - Function decleration
            //
            //   - Variable reassignment
            //   - Variable decleration
            //
            // So, lets see what we have after this, to assess what we're
            // even doing with this.
            //

            // Before we can do so, lets make sure we even have things after this
            if (!this->has_next(it, tokens.end())) {
                // We don't have anything next, so we need to display a friendly error.
                this->workspace.report_error({ "Incomplete identifier '" + token.value + "'", this->fileName, token.line, token.column });
                continue;
            }

            // Since we have something after this, lets see what we have.
            auto the_next_token = *(it + 1);
            if (the_next_token.type == TokenType::COLON) {
                // A colon here can mean two things:
                //   - Variable decleration WITH type
                //   - Variable decleration WITHOUT type
                //   - Function decleration
                //
                // We can figure this out based on what the token after this is.
                // If it's an '=', then we're going to infer the type.
                // If we find an identifier there, then we need to set the type
                // to that, if the type is valid.
                //

                if (!this->has_next(it + 1, tokens.end())) {
                    this->workspace.report_error({ "Incomplete variable decleration for '" + token.value + "'", this->fileName, token.line, token.column });
                    it += 3;
                    continue;
                }

                // Since we have something, lets check it out.
                auto next_token = *(it + 2);
                if (next_token.type == TokenType::ASSIGN) {
                    // Since it's an assign, we need to do one more check - Make sure there's a value!
                    if (!this->has_next(it + 2, tokens.end())) {
                        this->workspace.report_error({ "Missing value on variable decleration for '" + token.value  + "'", this->fileName, token.line, token.column });
                        it += 3;
                        continue;
                    }
                    // Make sure that this name hasn't been used
                    if (!this->can_use_name(token.value)) {
                        this->workspace.report_error({ "Variable name '" + token.value + "' is already in use", this->fileName, token.line, token.column });
                        it += 3;
                        continue;
                    }

                    // Since we have a value, we can turn this into a real variable!
                    auto value_token = *(it + 3);
                    std::string name = this->fileName + "_" + current_scope;
                    auto decl = new AST_Declaration(token.line, token.column, value_token.type, token.value, value_token.value, name);
                    if (this->scope_map[name]) this->scope_map[name]->contained.emplace_back(decl);
                    else file->contained.emplace_back(decl);
                    // Make sure the scanner knows it's been used too!
                    this->usedNames.emplace_back(token.value);
                }
                else if (the_next_token.type == TokenType::COLON && next_token.type == TokenType::COLON) {
                    // Having another colon here means we might be making a function.

                    // Make sure that we have something more to check.
                    if (!this->has_next(it + 2, tokens.end()) || !this->has_next(it + 3, tokens.end())) {
                        // We don't have at least two more, which we need.
                        this->workspace.report_error({ "Incomplete function decleration", this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    auto first_next = *(it + 3);
                    auto second_next = *(it + 4);

                    // Make sure we actually have what we need to make a function
                    if (first_next.type != TokenType::LPAREN) {
                        this->workspace.report_error({ "Function decleration is missing argument block", this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    current_scope = token.value;

                    // Since we have that, everything between that and ) is our arguments.
                    std::vector<AST_Argument> arguments;
                    auto argument_it = it + 4;

                    auto argument = *argument_it;
                    // COPYPASTA: This should be a function, was copied from another section
                    while (argument.type != TokenType::RPAREN && argument_it < tokens.end() - 1) {
                        // Build the AST representation of the argument from the token
                        
                        if (argument.type == TokenType::IDENTIFIER && this->has_next(argument_it, tokens.end())) {
                            auto colon = *(argument_it + 1);
                            if (colon.type == TokenType::COLON && this->has_next(argument_it + 1, tokens.end())) {
                                auto type = *(argument_it + 2);
                                auto real_type = get_type_from_string(type.value);
                                // Make sure it's a valid type
                                if (type.type != TokenType::IDENTIFIER || real_type == TokenType::INVALID) {
                                    this->workspace.report_error({ "'" + type.value + "' is an invalid type", file->file_name, type.line, type.column });
                                    argument_it += 2;
                                    it = argument_it;
                                    continue;
                                }
                                // It is, allow the function to have this argument as this type.
                                auto arg = new AST_Argument(argument.value, real_type);
                                arguments.emplace_back(*arg);
                                argument_it += 3;
                                argument = *argument_it;
                                it = argument_it;
                                continue;
                            }
                            std::cout << "A" << std::endl;
                            argument_it += 3;
                            argument = *argument_it;
                            it = argument_it;
                            continue;
                        }
                        std::cout << "B" << std::endl;
                        argument_it += 3;
                        argument = *argument_it;
                        it = argument_it;
                        continue;
                    }
                    // Set the iterator
                    this->next(arguments.size());

                    std::string name = this->fileName + "_" + current_scope;
                    // Now that we have our arguments, lets apply them to a function.
                    auto function = new AST_Function(token.value, token.line, token.column, arguments, name, TokenType::INVALID);

                    // Make sure we're not attempting to redeclare the main function
                    if (function->name == "main") {  // TODO: This shouldn't be static
                        if (file->mainFunction != nullptr) {
                            this->workspace.report_error({ "Attempt to redeclare main function", this->fileName, token.line, token.column });
                            it += 4;
                            continue;
                        }
                        // Set the main function to this
                        file->mainFunction = function;
                    }
                    // Make sure we're not redecling
                    bool skip_next = false;
                    for (auto& contained : file->contained) {
                        if (!is_type<AST_Function*>(contained)) continue;
                        auto func = static_cast<AST_Function*>(contained);
                        // Check the arguments
                        if (func->name == function->name && function->arguments.size() == func->arguments.size()) {
                            bool same_args = false;
                            for (auto& arg : function->arguments) {
                                for (auto& other_arg : func->arguments) {
                                    if (other_arg.type != arg.type) continue;
                                    same_args = true;
                                    break;
                                }
                            }
                            if (same_args) {
                                this->workspace.report_error({ "Attempt to declare function with same name and args as existing function.", this->fileName, token.line, token.column });
                                skip_next = true;
                                break;
                            }
                        }
                    }
                    if (skip_next) {
                        it += 4;
                        continue;
                    }
                    // Now that we have our function, give it to the file
                    file->contained.emplace_back(function);
                    this->scope_map[name] = function;
                }
                else if (next_token.type == TokenType::IDENTIFIER) {
                    // Having an identifier here means we're setting a custom type to a variable.
                    // So, we need to have 3 more arguments following.

                    if (!this->has_next(it + 1, tokens.end())) {
                        this->workspace.report_error({ "Expected '=', but found nothing", this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    auto type_token = *(it + 2);
                    // Lets make sure the type we have even exists.
                    auto new_type = get_type_from_string(type_token.value);
                    if (new_type == TokenType::INVALID) {
                        // The type isn't a thing
                        this->workspace.report_error({ "Invalid type: '" + type_token.value + "'", this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    // Since the type exists, lets see if we have a value.
                    if (!this->has_next(it + 3, tokens.end())) {
                        this->workspace.report_error({ "Cannot initialize variable without a value", this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }
                    
                    // We've got a value, create a new variable

                    // COPYPASTA:
                    // This is copied from an above if for initializing variables.
                    // This should become a nice generic function.
                    //
                    auto value_token = *(it + 4);

                    // Make sure that the types match
                    if (!this->workspace.typer.can_assign_this(new_type, value_token.type)) {
                        this->workspace.report_error({ "Cannot assign type '" + token_map[new_type] + "' from type '" + token_map[value_token.type] + "'",
                            this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    const auto name = this->fileName + "_" + current_scope;
                    auto decl = new AST_Declaration(token.line, token.column, value_token.type, token.value, value_token.value, name);
                    file->contained.emplace_back(decl);
                    if (this->scope_map[name]) this->scope_map[name]->contained.emplace_back(decl);
                    else file->contained.emplace_back(decl);
                    // Make sure the scanner knows it's been used too!
                    this->usedNames.emplace_back(token.value);
                    it += 4;
                }
            }
            else if (the_next_token.type == TokenType::ASSIGN) {
                // If we have a '=', then we're setting the value of a variable
                // that's already been declared.

                // Before we can, lets make sure we have something to actually assign it to
                if (!this->has_next(it + 1, tokens.end())) {
                    this->workspace.report_error({ "Cannot reassign variable without value", this->fileName, token.line, token.column });
                    it += 2;
                    continue;
                }
                // Since we have a value, lets see if the variable even exists
                if (this->can_use_name(token.value)) {
                    this->workspace.report_error({ "Cannot reassign variable '" + token.value + "', does not exist", this->fileName, token.line, token.column });
                    it += 2;
                    continue;
                }

                // Since we have both the value and a variable, set it.
                assert(original_decl)
                auto value_token = *(it + 2);

                // Make sure they're the same types
                if (!this->workspace.typer.can_assign_this(original_decl->type, value_token.type)) {
                    this->workspace.report_error({ "Cannot set variable '" + token.value + "', type " + token_map[original_decl->type] + ", to type of " +
                        token_map[value_token.type] + "", this->fileName, token.line, token.column });
                    it += 2;
                    continue;
                }
                // Types are the same, and the variable exists. Time to finally set it.
                original_decl->value = value_token.value;
            }
            else if (the_next_token.type == TokenType::LPAREN) {
                // If we have a left paren, then we MUST be calling a function.
                // So, lets take everything until ) as arguments.

                // But before we can do that, we need to make sure we even *have* args to do that with.
                if (!this->has_next(it + 1, tokens.end())) {
                    this->workspace.report_error({ "Expected (, found nothing", this->fileName, token.line, token.column });
                    it += 2;
                    continue;
                }

                std::vector<AST_Argument> arguments;
                auto argument_it = it + 2;

                Token argument = *argument_it;
                while (argument.type != TokenType::RPAREN && argument_it != tokens.end()) {
                    // Build the AST representation of the argument from the token
                    AST_Argument* arg = new AST_Argument(argument.value, argument.type);

                    arguments.emplace_back(*arg);
                    argument_it++;
                    argument = *argument_it;
                }
                // Set the iterator
                this->next(arguments.size());
                // Check if this is a builtin function
                AST_FunctionCall* function = nullptr;
                if (is_builtin(token.value)) function = new AST_Builtin(builtin_map[token.value], arguments);
                else function = new AST_FunctionCall(token.value, 0, 0, arguments);
                std::string name = this->fileName + "_" + current_scope;

                // Make sure that this isn't being attempted with native_
                if (Util::starts_with("native_", token.value)) {
                    if (!this->allow_native) {
                        // This isn't allowed. native_ is reserved for compiler directives
                        // that refer back to an action within C++.
                        this->workspace.report_error({ "Cannot declare function with native_ prefix", this->fileName, token.line, token.column });
                        continue;
                    }
                    // Otherwise, just mark this as native.
                    function->native = true;
                }
                if (this->scope_map[name]) this->scope_map[name]->contained.emplace_back(function);
                else file->contained.emplace_back(function);
            }
        }
    }
    return file;
}
