
#include "scanner.h"

Scanner::Scanner(WorkSpace& workspace) : workspace(workspace) {

}

Scanner::~Scanner() {

}

const std::string Scanner::read_string() {
    bool escaped = false;
    bool taking = false;
    std::string found = "";

    while (it < this->end) {
        if (escaped) {
            found += *it;
            escaped = false;
        }
        else if (*it == '\\' && this->has_next() && *(it + 1) == '"') escaped = true;
        else if (*it == '"') {
            if (!escaped && taking) taking = false;
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
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    std::vector<std::string> contents = Util::readFileToVector(fileName);
    std::vector<Token> tokens;
    if (contents.size() <= 0) return tokens;

    for (auto line : contents) {
        this->beg = line.begin();
        this->it = this->beg;
        this->end = line.end();

        std::string current = "";

        while (it < end) {
            if (*it == '"') {
                auto taken = this->read_string();
                Token token = { TokenType::STRING, taken };
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
                    Token identToken = { TokenType::IDENTIFIER, current };
                    tokens.emplace_back(identToken);
                    current = "";
                }

                Token token = { type, "" };
                tokens.emplace_back(token);
            }
            it++;
            continue;
        }
    }
    // Report the time it took
    auto nowTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    std::cout << "\033[1;36m> \033[0;32mFrontend time (s): " <<  ((double) (nowTime - time) / 1000000000) << "\033[0m" << std::endl;
    return tokens;
}

const SourceFile* Scanner::parse(std::vector<Token>& tokens) {
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    SourceFile* file = new SourceFile();

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
                // TODO: Real error reporting that actually helps the user solve the problem
                std::cout << "No useful information about the ident." << std::endl;
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

                // TODO: Support for type setting

                if (!this->has_next(it + 1, tokens.end())) {
                    // TODO: See the above TODO about error reporting.
                    std::cout << "Variable decleration isn't finished." << std::endl;
                    it += 3;
                    continue;
                }

                // Since we have something, lets check it out.
                auto next_token = *(it + 2);
                if (next_token.type == TokenType::ASSIGN) {
                    // Since it's an assign, we need to do one more check - Make sure there's a value!
                    if (!this->has_next(it + 2, tokens.end())) {
                        // TODO: See the above TODO about error reporting
                        std::cout << "Variable decleration has no value" << std::endl;
                        it += 3;
                        continue;
                    }
                    // Make sure that this name hasn't been used
                    if (!this->can_use_name(token.value)) {
                        // TODO: See above TODO about error reporting
                        std::cout << "Cannot use name '" << token.value << "'." << std::endl;
                        it += 3;
                        continue;
                    }

                    // Since we have a value, we can turn this into a real variable!
                    auto value_token = *(it + 3);
                    Decleration decl = {
                        0, 0, value_token.type, token.value, value_token.value, "cool_scope" // TODO: Real scopes
                    };
                    file->decls.emplace_back(decl);
                    // Make sure the scanner knows it's been used too!
                    this->usedNames.emplace_back(token.value);
                }
                else if (the_next_token.type == TokenType::COLON && next_token.type == TokenType::COLON) {
                    // Having another colon here means we might be making a function.

                    // Make sure that we have something more to check.
                    if (!this->has_next(it + 2, tokens.end()) || !this->has_next(it + 3, tokens.end())) {
                        // We don't have at least two more, which we need.
                        
                        // TODO: See above TODO about error reporting
                        std::cout << "Function format issue" << std::endl;
                        it += 4;
                        continue;
                    }

                    auto first_next = *(it + 3);
                    auto second_next = *(it + 4);

                    // Make sure we actually have what we need to make a function
                    if (first_next.type != TokenType::LPAREN) {
                        // TODO: See above TODO about error reporting
                        std::cout << "Function format issue" << std::endl;
                        it += 4;
                        continue;
                    }

                    // Since we have that, everything between that and ) is our arguments.
                    std::vector<Token> arguments;
                    auto argument_it = it + 4;

                    auto argument = *argument_it;
                    // COPYPASTA: This should be a function, was copied from another section
                    while (argument.type != TokenType::RPAREN && argument_it != tokens.end()) {
                        arguments.emplace_back(argument);
                        argument_it++;
                        argument = *argument_it;
                    }
                    // Set the iterator
                    it += arguments.size();

                    // Now that we have our arguments, lets apply it to a function.
                    Function function = {  };
                    function.column = 0;
                    function.line = 0;
                    function.arguments = arguments;
                    function.function_name = token.value;
                    function.isMain = true; // TODO
                    function.parent_scope = ""; // TODO
                    function.return_type = TokenType::INT; // TODO
                    function.scope = ""; // TODO

                    // Now that we have our function, give it to the file
                    file->functions.emplace_back(function);
                }
                else if (next_token.type == TokenType::IDENTIFIER) {
                    // Having an identifier here means we're setting a custom type to a variable.
                    // So, we need to have 3 more arguments following.

                    if (!this->has_next(it + 2, tokens.end())) {
                        // TODO: See above TODO about error reporting
                        std::cout << "Expected an '='" << std::endl;
                        it += 4;
                        continue;
                    }

                    auto type_token = *(it + 2);
                    // Lets make sure the type we have even exists.
                    auto new_type = get_type_from_string(type_token.value);
                    if (new_type == TokenType::INVALID) {
                        // The type isn't a thing
                        std::cout << "Invalid type: '" << type_token.value << "'" << std::endl;
                        it += 4;
                        continue;
                    }

                    // Since the type exists, lets see if we have a value.
                    if (!this->has_next(it + 3, tokens.end())) {
                        // TODO: See above TODO about error reporting
                        std::cout << "Cannot initialize variable without value." << std::endl;
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
                        // TODO: See above TODO about error reporting
                        std::cout << "Cannot assign with two different types." << std::endl;
                        it += 4;
                        continue;
                    }
                    
                    Decleration decl = {
                        0, 0, value_token.type, token.value, value_token.value, "cool_scope" // TODO: Real scopes
                    };
                    file->decls.emplace_back(decl);
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
                    // TODO: See above TODO about error reporting
                    std::cout << "Cannot set value without a value!" << std::endl;
                    it += 2;
                    continue;
                }
                // Since we have a value, lets see if the variable even exists
                if (this->can_use_name(token.value)) {
                    // Variable doesn't exist
                    std::cout << "Variable '" << token.value << "' does not exist." << std::endl;
                    it += 2;
                    continue;
                }

                // Since we have both the value and a variable, set it.
                auto originalDecl = file->getDecl(token.value);
                if (originalDecl.line == -1) {
                    std::cout << "Internal compiler error: Found name that doesn't exist: " << token.value << std::endl;
                    break;
                }
                auto value_token = *(it + 2);

                // Make sure they're the same types
                if (originalDecl.type != value_token.type) {
                    std::cout << "Cannot set variable '" << token.value << "', type " << token_map[originalDecl.type] << ", to type of " << token_map[value_token.type] << "." << std::endl;
                    it += 2;
                    continue;
                }
                // Types are the same, and the variable exists. Time to finally set it.
                auto newDecl = originalDecl;
                newDecl.value = value_token.value;
                file->replaceDecl(originalDecl, newDecl);
            }
            else if (the_next_token.type == TokenType::LPAREN) {
                // If we have a left paren, then we MUST be calling a function.
                // So, lets take everything until ) as arguments.

                // But before we can do that, we need to make sure we even *have* args to do that with.
                if (!this->has_next(it + 1, tokens.end())) {
                    // TODO: See above TODO about error reporting.
                    std::cout << "Function call doesn't have args / end" << std::endl;
                    it += 2;
                    continue;
                }

                std::vector<Token> arguments;
                auto argument_it = it + 2;

                Token argument = *argument_it;
                while (argument.type != TokenType::RPAREN && argument_it != tokens.end()) {
                    arguments.emplace_back(argument);
                    argument_it++;
                    argument = *argument_it;
                }
                // Set the iterator
                it += arguments.size();
                
                // Now that we have our arguments, let build the AST function call.
                FunctionCall function = { 0, 0, token.value, arguments };
                file->function_calls.emplace_back(function);
            }
        }
    }

    // Report the time it took
    auto nowTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    std::cout << "\033[1;36m> \033[0;32mBackend time  (s): " << ((double)(nowTime - time) / 1000000000) << "\033[0m" << std::endl;
    return file;
}
