
#include "scanner.h"

Scanner::Scanner(WorkSpace& workspace) : workspace(workspace) {

}

Scanner::~Scanner() {

}

const std::string Scanner::read_string(const char& delim) {
    bool escaped = false;
    bool taking = false;

    std::string found = "";

    while (it < this->end) {
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

    return found;
}

const Token Scanner::read_number() {
    std::string found = "";
    bool encountered_dot = false;

    while (it < this->end) {
        if (*it == '.') encountered_dot = true;
        else if (!Util::is_number(*it)) break;
        found += *it;
        it++;
    }
    Token token = {};
    token.type = encountered_dot ? TokenType::FLOAT : TokenType::INT;
    token.value = found;
    return token;
}

const bool Scanner::check_comment() {
    // Clone the iterator
    auto i = this->it;

    bool all_have_matched = true;

    // Make sure the iterator starts with the whole comment prefix
    for (auto c : this->workspace.comment_prefix) {
        if (!has_next(i)) return false;
        all_have_matched = *i == c;
        i++;
    }

    return all_have_matched;
}

const bool Scanner::can_use_name(std::string name) {
    // TODO: This needs more checking for the contents of the name too.
    // We don't want names like: `t-h-e_t-h-i-n-g`. 
    return !Util::vectorContains(this->usedNames, name) && !Util::vectorContains(keywords, name);
}

const std::vector<Token> Scanner::tokenize(const std::string& fileName) {
    this->fileName = fileName;

    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    std::vector<std::string> contents = Util::readFileToVector(fileName);
    std::vector<Token> tokens;
    if (contents.size() <= 0) return tokens;

    int line_num = 1;
    for (auto line : contents) {
        int column = 1;
        this->beg = line.begin();
        this->it = this->beg;
        this->end = line.end();

        std::string current = "";

        while (it < end) {
            if (*it == '"') {
                auto taken = this->read_string('"');
                Token token = { TokenType::STRING, taken, line_num, column };
                tokens.emplace_back(token);
                continue;
            } else if (this->check_comment()) {
                it += 2;
                continue;
            }
            else if (Util::is_number(*it)) {
                auto token = this->read_number();
                tokens.emplace_back(token);
                continue;
            }
            else {
                std::string a;
                a += *it;

                if (a == " " || a == "") {
                    it++;
                    continue;
                }

                // Get the type of the character we're currently looking at
                auto type = get_token_type(a);

                // If it's an ident, that means it's not something we have.
                // So, add that to our currently found (for later use) string.
                if (type == TokenType::IDENTIFIER && (a != " " && a != "")) {
                    current += *it;
                    it++;
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
            it++;
            column++;
        }
        line_num++;
    }
    // Report the time it took
    auto nowTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    std::cout << "\033[1;36m> \033[0;32mFrontend time (s): " <<  ((double) (nowTime - time) / 1000000000) << "\033[0m" << std::endl;
    return tokens;
}

const AST_SourceFile* Scanner::parse(std::vector<Token>& tokens) {
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    AST_SourceFile* file = new AST_SourceFile(this->fileName);

    // Start by making sure we even have any tokens to parse.
    if (tokens.size() == 0) return file;

    // We have things to check, so lets just start going through.
    for (auto it = tokens.begin(); it < tokens.end(); ++it) {
        auto token = *it;
        // Check the type of the token
        
        if (token.type == TokenType::IDENTIFIER) {
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
                this->workspace.reporter.report_error({ "Incomplete identifier", this->fileName, token.line, token.column });
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
                    this->workspace.reporter.report_error({ "Incomplete variable decleration", this->fileName, token.line, token.column });
                    it += 3;
                    continue;
                }

                // Since we have something, lets check it out.
                auto next_token = *(it + 2);
                if (next_token.type == TokenType::ASSIGN) {
                    // Since it's an assign, we need to do one more check - Make sure there's a value!
                    if (!this->has_next(it + 2, tokens.end())) {
                        this->workspace.reporter.report_error({ "Missing value on variable decleration", this->fileName, token.line, token.column });
                        it += 3;
                        continue;
                    }
                    // Make sure that this name hasn't been used
                    if (!this->can_use_name(token.value)) {
                        this->workspace.reporter.report_error({ "Variable name '" + token.value + "' is already in use", this->fileName, token.line, token.column });
                        it += 3;
                        continue;
                    }

                    // Since we have a value, we can turn this into a real variable!
                    auto value_token = *(it + 3);
                    auto decl = new AST_Declaration(token.line, token.column, value_token.type, token.value, value_token.value, "cool_scope");
                    file->contained.emplace_back(decl);
                    // Make sure the scanner knows it's been used too!
                    this->usedNames.emplace_back(token.value);
                }
                else if (the_next_token.type == TokenType::COLON && next_token.type == TokenType::COLON) {
                    // Having another colon here means we might be making a function.

                    // Make sure that we have something more to check.
                    if (!this->has_next(it + 2, tokens.end()) || !this->has_next(it + 3, tokens.end())) {
                        // We don't have at least two more, which we need.
                        this->workspace.reporter.report_error({ "Incomplete function decleration", this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    auto first_next = *(it + 3);
                    auto second_next = *(it + 4);

                    // Make sure we actually have what we need to make a function
                    if (first_next.type != TokenType::LPAREN) {
                        this->workspace.reporter.report_error({ "Function decleration is missing argument block", this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    // Since we have that, everything between that and ) is our arguments.
                    std::vector<AST_Argument> arguments;
                    auto argument_it = it + 4;

                    auto argument = *argument_it;
                    // COPYPASTA: This should be a function, was copied from another section
                    while (argument.type != TokenType::RPAREN && argument_it != tokens.end()) {
                        // Build the AST representation of the argument from the token
                        // Cleanup: Literally what even
                        AST_Argument arg;
                        arg.name = argument.value;
                        arg.type = argument.type;

                        arguments.emplace_back(arg);
                        argument_it++;
                        argument = *argument_it;
                    }
                    // Set the iterator
                    it += arguments.size();

                    // Now that we have our arguments, lets apply it to a function.
                    auto function = new AST_Function(token.value, token.line, token.column, arguments, "really_cool_scope", TokenType::INVALID);

                    // Make sure we're not attempting to redeclare the main function
                    if (function->name == "main") {  // TODO: This shouldn't be static
                        if (file->mainFunction != nullptr) {
                            this->workspace.reporter.report_error({ "Attempt to redeclare main function", this->fileName, token.line, token.column });
                            it += 4;
                            continue;
                        }
                        // Set the main function to this
                        file->mainFunction = function;
                    }
                    // Now that we have our function, give it to the file
                    file->contained.emplace_back(function);
                }
                else if (next_token.type == TokenType::IDENTIFIER) {
                    // Having an identifier here means we're setting a custom type to a variable.
                    // So, we need to have 3 more arguments following.

                    if (!this->has_next(it + 2, tokens.end())) {
                        this->workspace.reporter.report_error({ "Expected '=', but found nothing", this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    auto type_token = *(it + 2);
                    // Lets make sure the type we have even exists.
                    auto new_type = get_type_from_string(type_token.value);
                    if (new_type == TokenType::INVALID) {
                        // The type isn't a thing
                        this->workspace.reporter.report_error({ "Invalid type: '" + type_token.value + "'", this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    // Since the type exists, lets see if we have a value.
                    if (!this->has_next(it + 3, tokens.end())) {
                        this->workspace.reporter.report_error({ "Cannot initialize variable without a value", this->fileName, token.line, token.column });
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
                    if (new_type != value_token.type) {
                        this->workspace.reporter.report_error({ "Cannot assign type '" + token_map[new_type] + "' from type '" + token_map[value_token.type] + "'", this->fileName, token.line, token.column });
                        it += 4;
                        continue;
                    }
                    
                    auto decl = new AST_Declaration(token.line, token.column, value_token.type, token.value, value_token.value, "cool_scope");
                    file->contained.emplace_back(decl);
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
                    this->workspace.reporter.report_error({ "Cannot reassign variable without value", this->fileName, token.line, token.column });
                    it += 2;
                    continue;
                }
                // Since we have a value, lets see if the variable even exists
                if (this->can_use_name(token.value)) {
                    this->workspace.reporter.report_error({ "Cannot reassign variable '" + token.value + "', does not exist", this->fileName, token.line, token.column });
                    it += 2;
                    continue;
                }

                // Since we have both the value and a variable, set it.
                auto original_decl = file->get_decl(token.value);
                if (original_decl->line == -1) {
                    this->workspace.reporter.report_error({ "Internal compiler error: Found name that doesn't exist: " + token.value, this->fileName, token.line, token.column });
                    break;
                }
                auto value_token = *(it + 2);

                // Make sure they're the same types
                if (original_decl->type != value_token.type) {
                    this->workspace.reporter.report_error({ "Cannot set variable '" + token.value + "', type " + token_map[original_decl->type] + ", to type of " +
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
                    this->workspace.reporter.report_error({ "Expected (, found nothing", this->fileName, token.line, token.column });
                    it += 2;
                    continue;
                }

                std::vector<AST_Argument> arguments;
                auto argument_it = it + 2;

                Token argument = *argument_it;
                while (argument.type != TokenType::RPAREN && argument_it != tokens.end()) {
                    // Build the AST representation of the argument from the token
                    // Cleanup: Literally what even
                    AST_Argument arg;
                    arg.name = argument.value;
                    arg.type = argument.type;

                    arguments.emplace_back(arg);
                    argument_it++;
                    argument = *argument_it;
                }
                // Set the iterator
                it += arguments.size();
                // Now that we have our arguments, let build the AST function call.
                auto function = new AST_FunctionCall(token.value, 0, 0, arguments);
                file->contained.emplace_back(function);
            }
        }
    }

    // Report the time it took
    auto nowTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    std::cout << "\033[1;36m> \033[0;32mBackend time  (s): " << ((double)(nowTime - time) / 1000000000) << "\033[0m" << std::endl;
    return file;
}
