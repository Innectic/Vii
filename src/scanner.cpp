
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
    Token token = { TokenType::INVALID, "", 0, 0, ASTFlag::TOKEN };

    while (it <= this->end) {
        if (it == this->end && *(it - 1) != delim) {
            // This means it was never teminated...
            was_terminated = false;
            break;
        }

        if (escaped) {
            found += *it;
            escaped = false;
        } else if (*it == '\\' && this->has_next() && *(it + 1) == delim) escaped = true;
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

    if (token.type == TokenType::CHAR && token.value.size() != 1) {
        token.type = TokenType::INVALID;
        token.value = "";
		token.flag = ASTFlag::UNPROCESSED;
    }

    return token;
}

const Token Scanner::read_number() {
    std::string found = "";
    bool encountered_dot = false;
    bool invalid = false;

    Token token = { TokenType::INVALID, "0", 0, 0, ASTFlag::UNPROCESSED };

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
        it++;
    }
    if (!invalid) {
        token.value = found;
        token.type = encountered_dot ? TokenType::FLOAT : TokenType::INT;
		token.flag = ASTFlag::TOKEN;
	}
    return token;
}

const bool Scanner::check_comment() {
    if (!has_next(this->it)) return false;
    auto next = *(it + 1);
    return *it == '/' && next == '/';
}

AST_Math* Scanner::do_math(std::vector<Token>::iterator& end, std::vector<Token>::iterator& it, const Token& token) {
    if (!this->has_next(it, end) || !this->has_next(it + 1, end)) return nullptr;
    std::vector<AST_Operation> operations;

	auto typer = this->workspace.typer;

    bool chain = false;
    while (this->has_next(it, end)) {
        if (!this->has_next(it + 1, end) || !this->has_next(it + 2, end)) break;
        auto operator_token = it + 1;
        auto next_adding = it + 2;

		assert(operator_token->flag == ASTFlag::SCAN);
		assert(next_adding->flag == ASTFlag::SCAN);

        if (!typer.is_operator(operator_token->type)) {
            this->workspace.report_error({ "Invalid operator type: " + token_map[operator_token->type], this->file_name, token.line, token.column });
            it += 2;
            continue;
        }

        if (!typer.is_numeric(it->type)) {
            this->workspace.report_error({ "Cannot add this type: " + token_map[it->type], this->file_name, token.line, token.column });
            it += 2;
            continue;
        }

        if (!typer.is_numeric(next_adding->type)) {
            this->workspace.report_error({ "Cannot add this type: " + token_map[next_adding->type], this->file_name, token.line, token.column });
            it += 2;
            continue;
        }

        AST_Operation operation = { it->value, it->type, next_adding->value, next_adding->type, operator_token->type, chain };
		operation.flag = ASTFlag::SCAN;
        operations.emplace_back(operation);
        it += 2;
        if (!typer.is_operator((it + 1)->type)) break;
        if (!chain) chain = true;
    }

    if (operations.size() == 0) {
        this->workspace.report_error({ "INTERNAL COMPILER ERROR: Didn't get any operations / operators from expression?!", this->file_name, token.line, token.column });
        return nullptr;
    }
    return new AST_Math(token.line, token.column, operations, "");
}

const bool Scanner::can_use_name(const std::string& name) {
    return !Util::vector_contains(this->used_names, name) && this->workspace.typer.get_keyword_type(name) == KeywordType::INVALID;
}

const std::vector<Token> Scanner::tokenize(const std::string& file_name, const bool allow_native) {
    this->file_name = file_name;
    this->allow_native = allow_native;

	auto typer = this->workspace.typer;

    std::vector<std::string> contents = Util::read_file_to_vector(file_name);
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
                it++;
                continue;
            }
            if (this->check_comment()) break;
            else if (*it == '/' && this->has_next() && *(it + 1) == '*' && !skip_next) {
                skip_next = true;
                it++;
                continue;
            } else if (*it == '"' || *it == '\'') {
                auto token = this->read_string(*it);
                if (token.type == TokenType::INVALID) {
                    if (token.value == "") {
                        this->workspace.report_error({ "Char must contain only one character.", this->file_name, 0, column });
                        continue;
                    }
                    this->workspace.report_error({ "String / char was never terminated.", this->file_name, 0, column });
                    continue;
                }
				token.flag = ASTFlag::TOKEN;
				tokens.emplace_back(token);
                continue;
            } else if (Util::is_number(*it)) {
                auto token = this->read_number();
                if (token.type == TokenType::INVALID) {
                    this->workspace.report_error({ "Number cannot contain multiple '.'", this->file_name, 0, column });
                    continue;
                }
				token.flag = ASTFlag::TOKEN;
                tokens.emplace_back(token);
                continue;
            }
            std::string a;
            a += *it;

            if (a == " " || a == "" || a == "\t") {
                it++;
                continue;
            }

            // Get the type of the character we're currently looking at
            auto type = typer.get_token_type(a);

            // If it's an ident, that means it's not something we have.
            // So, add that to our currently found (for later use) string.
            if (type == TokenType::IDENTIFIER && (a != " " && a != "")) {
                current += *it;
                it++;
                continue;
            }

            if (current != "" && current != " ") {
                Token identToken = { TokenType::IDENTIFIER, current, line_num, column, ASTFlag::TOKEN };
                tokens.emplace_back(identToken);
                current = "";
            }

            Token token = { type, "", line_num, column, ASTFlag::TOKEN };
            tokens.emplace_back(token);
            
            it++;
            column++;
        }
        if (current != "") {
            auto type = this->workspace.typer.get_token_type(current);
            Token remaining_token = { type, current, line_num, column, ASTFlag::TOKEN };
            tokens.emplace_back(remaining_token);
        }
        line_num++;
    }
    return tokens;
}

AST_SourceFile* Scanner::parse(std::vector<Token>& tokens, bool scoped) {
	return this->parse(tokens.begin(), tokens.end(), scoped);
}

AST_SourceFile* Scanner::parse(std::vector<Token>::iterator start, std::vector<Token>::iterator end, bool scoped) {
	AST_SourceFile* file = new AST_SourceFile(this->file_name);

	auto typer = this->workspace.typer;

    // Start by making sure we even have any tokens to parse.
    std::string current_scope = "";

    // We have things to check, so lets just start going through.
    for (auto it = start; it < end - 1; ++it) {
		this->hack = it;
		auto token = *it;

		assert(token.flag == ASTFlag::TOKEN);
		token.flag = ASTFlag::SCAN;

		// Check if it's a keyword.
		auto keyword_type = typer.get_keyword_type(token.value);
		if (keyword_type != KeywordType::INVALID) {
			// Since this is a keyword, then we need to figure out
			// what kind it is, and how to proceed.
			//
			switch (keyword_type) {
				case KeywordType::IF: {
					// Make sure there's at least a possible conditional
					if (!this->has_next(it, end)) {
						// If statement is missing conditional.
						ViiError error = {
							"If statement is missing condition.",
							this->file_name,
							token.line,
							token.column
						};
						this->workspace.report_error(error);
						continue;
					}
					// We potentially have a conditional!
					auto position = it + 1;
					//
					// Essentially, we're trying to match this:
					//
					// if [conditional] {
					//     // something
					// }
					//
					std::string conditional = (it - 1)->value;
					std::vector<AST_Type*> contained;

					auto finished_conditional = false;

					while (this->has_next(position, end)) {
						// If we're still trying to finish the conditional, then we want to just add on to it.
						// However, if we have the conditional then we just need to pull the body of the statement.
						//
						if (!finished_conditional) {
							if (position->type == TokenType::LBRACE) {
								// We've finished the conditional, time to get the body.
								finished_conditional = true;
								continue;
							}

							if (typer.is_conditional_operator(position->type)) conditional += typer.conditional_operator_as_string(position->type);
							else conditional += position->value + " ";
							position++;
							continue;
						}
						conditional = Util::trim(conditional);
						
						// Since the conditional is finished, lets take the body.
						if (position->type == TokenType::RBRACE) break;
						auto result = this->parse(position + 1, end, false);
						position = this->hack; // HACK

						auto main_if = new AST_If(
							position->line,
							position->column,
							conditional,
							result
						);

						auto if_block = new AST_If_Block();
						if_block->first_block = main_if;

						auto name = this->file_name + "$" + current_scope;
						if (scoped) this->add_scoped(name, if_block);
						else file->contained.emplace_back(if_block);

						position++;
					}

					it = position;
					continue;
				}
				case KeywordType::TRUE:
				case KeywordType::FALSE: {
					assert(typer.is_token_boolean(it->value));
					auto value = typer.string_to_bool(it->value);

					auto ast_bool = new AST_Bool(value);
					auto name = this->file_name + "$" + current_scope;

					if (scoped) this->add_scoped(name, ast_bool);
					else file->contained.emplace_back(ast_bool);
					continue;
				}
			}
		}

		// Check the type of the token
		if (token.type == TokenType::RBRACE) {
            current_scope = "";
            continue;
        } else if (token.type == TokenType::IDENTIFIER) {
            // If we have an identifier type, this can be four different things.
            //   - Function call
            //   - Function decleration
            //
            //   - Variable reassignment
            //   - Variable decleration
            //
            // So, lets see what we have after this, to assess what we're
            // even doing.
            //

            // Before we can do so, lets make sure we even have things after this
            if (!this->has_next(it, end)) {
                // We don't have anything next, so we need to display a friendly error.
                this->workspace.report_error({ "Incomplete identifier", this->file_name, token.line, token.column });
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

                if (!this->has_next(it + 1, end)) {
                    this->workspace.report_error({ "Incomplete variable decleration", this->file_name, token.line, token.column });
                    it += 3;
                    continue;
                }

                // Since we have something, lets check it out.
                auto next_token = *(it + 2);
                if (next_token.type == TokenType::ASSIGN) {
                    // Since it's an assign, we need to do one more check - Make sure there's a value!
                    if (!this->has_next(it + 2, end)) {
                        this->workspace.report_error({ "Missing value on variable decleration", this->file_name, token.line, token.column });
                        it += 3;
                        continue;
                    }
                    // Make sure that this name hasn't been used
                    if (!this->can_use_name(token.value)) {
                        this->workspace.report_error({ "Variable name '" + token.value + "' is already in use", this->file_name, token.line, token.column });
                        it += 3;
                        continue;
                    }

                    auto has_math = false;
                    AST_Math* math = nullptr;
                    if (typer.is_operator((it + 4)->type)) {
                        // TODO: This won't allow things like idents to be used for evaluating things.
                        // We'll do math to set the value of this.
                        auto evaluated = this->do_math(end, it + 3, token);
                        if (evaluated) {
                            has_math = true;
                            math = evaluated;
                        }
                    }

                    // Since we have a value, we can turn this into a real variable!
                    auto value_token = *(it + 3);
                    // TODO: Ew
                    AST_Declaration* decl = nullptr;
                    auto name = this->file_name + "$" + current_scope;
                    if (has_math) {
                        math->scope = name;
                        decl = new AST_Declaration(token.line, token.column, value_token.type, token.value, math, name);
                    }
					else {
						decl = new AST_Declaration(token.line, token.column, value_token.type, token.value, value_token.value, name);
						decl->math = nullptr;
					}
                    // Make sure everything exists.

					decl->flag = ASTFlag::SCAN;
					assert(decl);
                    assert(decl->math || decl->value != "");
                    // Make sure the scanner knows it's been used too!
                    this->used_names.emplace_back(token.value);
                    if (scoped) this->add_scoped(name, decl);
					else file->contained.emplace_back(decl);
                } else if (the_next_token.type == TokenType::COLON && next_token.type == TokenType::COLON) {
                    // Having another colon here means we might be making a function.

                    // Make sure that we have something more to check.
                    if (!this->has_next(it + 2, end) || !this->has_next(it + 3, end)) {
                        // We don't have at least two more, which we need.
                        this->workspace.report_error({ "Incomplete function decleration", this->file_name, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    auto first_next = *(it + 3);
                    auto second_next = *(it + 4);

                    // Make sure we actually have what we need to make a function
                    if (first_next.type != TokenType::LPAREN) {
                        this->workspace.report_error({ "Function decleration is missing argument block", this->file_name, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    // Since we have that, everything between that and ) is our arguments.
                    std::vector<AST_Argument> arguments;
                    it += 4;

					bool should_be_looking_at_comma = true;
					// COPYPASTA: This should be a function, was copied from another section
                    while (it->type != TokenType::RPAREN && this->has_next(it, end)) {
                        // Build the AST representation of the argument from the token
						switch (it->type) {
							case TokenType::IDENTIFIER: {
								if (!should_be_looking_at_comma) {
									ViiError error = {
										"Expecting a comma between arguments.",
										file->file_name,
										it->line,
										it->column
									};
									this->workspace.report_error(error);
									continue;
								}
								if (!this->has_next(it, end)) continue;
								if ((it + 1)->type != TokenType::COLON) continue;
								if (!this->has_next(it + 1, end)) continue;

								auto type = it + 2;
								auto real_type = typer.get_type_from_string(type->value);

								if (type->type != TokenType::IDENTIFIER || real_type == TokenType::INVALID) {
									ViiError error = {
										"Invalid type: " + type->value,
										file->file_name,
										type->line,
										type->column
									};
									this->workspace.report_error(error);
									it += 2;
									continue;
								}
								auto arg = new AST_Argument(it->value, real_type);
								arguments.emplace_back(*arg);
								it += 3;
								should_be_looking_at_comma = false;
								break;
							}
							case TokenType::COMMA: {
								should_be_looking_at_comma = true;
                                if (this->has_next(it, end)) it ++;
								break;
							}
							default:
								std::cout << "How did this happen" << std::endl;
								break;
						}
                    }
                    // Set the iterator
                    this->next(arguments.size());

                    std::string name = this->file_name + "$" + current_scope;
                    // Now that we have our arguments, lets apply them to a function.
                    auto function = new AST_Function(token.value, token.line, token.column, arguments, name, TokenType::INVALID);

                    if (function->name == "main") {
						// Make sure we're not attempting to redeclare the main function
						if (file->mainFunction) {
                            this->workspace.report_error({ "Attempt to redeclare main function", this->file_name, token.line, token.column });
                            it += 4;
                            continue;
                        }
                        // Set the main function to this
                        file->mainFunction = function;
                    }
                    // Now that we have our function, give it to the file
					function->flag = ASTFlag::SCAN;
					if (scoped) this->set_scope(name, function);
					else file->contained.emplace_back(function);

					// TODO: go bye-bye?
                    file->contained.emplace_back(function);

                    current_scope = function->contained_scope;
                }
                else if (next_token.type == TokenType::IDENTIFIER) {
                    // Having an identifier here means we're setting a custom type to a variable.
                    // So, we need to have 3 more arguments following.

                    auto has_math = false;
                    AST_Math* math = nullptr;
                    if (typer.is_operator((it + 5)->type)) {
                        // TODO: This won't allow things like idents to be used for evaluating things.
                        // We'll do math to set the value of this.
                        auto evaluated = this->do_math(end, it + 4, token);
                        if (evaluated) {
                            has_math = true;
                            math = evaluated;
                        }
                    }

                    if (!this->has_next(it + 1, end)) {
                        this->workspace.report_error({ "Expected '=', but found nothing", this->file_name, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    auto type_token = *(it + 2);
                    // Lets make sure the type we have even exists.
                    auto new_type = typer.get_type_from_string(type_token.value);
                    if (new_type == TokenType::INVALID) {
                        // The type isn't a thing
                        this->workspace.report_error({ "Invalid type: '" + type_token.value + "'", this->file_name, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    // Since the type exists, lets see if we have a value.
                    if (!this->has_next(it + 3, end)) {
                        this->workspace.report_error({ "Cannot initialize variable without a value", this->file_name, token.line, token.column });
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
                            this->file_name, token.line, token.column });
                        it += 4;
                        continue;
                    }

                    AST_Declaration* decl = nullptr;
                    auto name = this->file_name + "$" + current_scope;
                    if (has_math) {
                        math->scope = name;
                        decl = new AST_Declaration(token.line, token.column, value_token.type, token.value, math, name);
                    }
					else {
						decl = new AST_Declaration(token.line, token.column, value_token.type, token.value, value_token.value, name);
						decl->math = nullptr;
					}
                    // Checks
					decl->flag = ASTFlag::SCAN;
					assert(decl);
                    assert(decl->math || decl->value != "");
                    // Make sure the scanner knows it's been used too!
                    this->used_names.emplace_back(token.value);
                    if (scoped) this->add_scoped(name, decl);
					else file->contained.emplace_back(decl);
					it += 4;
                }
            }
            else if (the_next_token.type == TokenType::ASSIGN) {
                // If we have a '=', then we're setting the value of a variable
                // that's already been declared.

                auto has_math = false;
                AST_Math* math = nullptr;
                if (typer.is_operator((it + 3)->type)) {
                    // TODO: This won't allow things like idents to be used for evaluating things.
                    // We'll do math to set the value of this.
                    auto evaluated = this->do_math(end, it + 2, token);
                    if (evaluated) {
                        has_math = true;
                        math = evaluated;
						assert(math);
					}
				}

                // Before we can, lets make sure we have something to actually assign it to
                if (!this->has_next(it + 1, end)) {
                    this->workspace.report_error({ "Cannot reassign variable without value", this->file_name, token.line, token.column });
                    it += 2;
                    continue;
                }
                // Since we have a value, lets see if the variable even exists
                if (this->can_use_name(token.value)) {
                    this->workspace.report_error({ "Cannot reassign variable '" + token.value + "', does not exist", this->file_name, token.line, token.column });
                    it += 2;
                    continue;
                }

                // Since we have both the value and a variable, set it.
                auto original_decl = file->get_decl(token.value);
                if (!original_decl) {
                    this->workspace.report_error({ "Internal compiler error: Found name that doesn't exist: " + token.value, this->file_name, token.line, token.column });
                    break;
                }
                auto value_token = *(it + 2);

                // Make sure they're the same types
                if (!typer.can_assign_this(original_decl->type, value_token.type)) {
                    this->workspace.report_error({ "Cannot set variable '" + token.value + "', type " + token_map[original_decl->type] + ", to type of " +
                        token_map[value_token.type] + "", this->file_name, token.line, token.column });
                    it += 2;
                    continue;
                }
                auto decl = new AST_Declaration(token.line, token.column, original_decl->type, original_decl->name, nullptr, original_decl->scope, true);
                // Types are the same, and the variable exists. Time to finally set it.
                if (has_math) decl->math = math;
				else {
					decl->value = value_token.value;
					decl->math = nullptr;
				}
				decl->flag = ASTFlag::SCAN;
				// Checks
				assert(decl);
				assert(decl->math || decl->value != "");

                auto name = decl->scope;
				if (scoped) {}
				else file->contained.emplace_back(decl);
            } else if (the_next_token.type == TokenType::LPAREN) {
                // If we have a left paren, then we MUST be calling a function.
                // So, lets take everything until ) as arguments.

                // But before we can do that, we need to make sure we even *have* args to do that with.
                if (!this->has_next(it + 1, end)) {
                    this->workspace.report_error({ "Expected (, found nothing", this->file_name, token.line, token.column });
                    it += 2;
                    continue;
                }

                std::vector<AST_Argument> arguments;
                it += 2;

				bool expecting_comma = false;
                while (it->type != TokenType::RPAREN && this->has_next(it, end)) {
					if (expecting_comma) {
						if (it->type == TokenType::COMMA) {
							expecting_comma = false;
							// Expecting a comma, found one.
							it++;
							continue;
						} else {
							// Expecting a comma, didn't find one.
							this->workspace.report_error({ "Expected a comma between arguments.", this->file_name, token.line, token.column });
							it++;
							break;
						}
					} else {
						// Function call's argument, nothing special here.
						AST_Argument* arg = new AST_Argument(it->value, it->type);

						arguments.emplace_back(*arg);
						expecting_comma = true;
						it++;
					}
                }
                // Check if this is a builtin function
                auto name = this->file_name + "$" + current_scope;
                AST_FunctionCall* function = nullptr;
                if (typer.is_builtin(token.value)) function = new AST_Builtin(builtin_map[token.value], arguments);
                else function = new AST_FunctionCall(token.value, 0, 0, arguments, name);
				assert(function);

                // Make sure that this isn't being attempted with native_
                if (Util::starts_with("native_", token.value)) {
                    if (!this->allow_native) {
                        // This isn't allowed. native_ is reserved for compiler directives
                        // that refer back to an action within C++.
                        this->workspace.report_error({ "Cannot declare function with native_ prefix", this->file_name, token.line, token.column });
                        continue;
                    }
                    // Otherwise, just mark this as native.
                    function->native = true;
                }
				if (scoped) this->add_scoped(name, function);
				else file->contained.emplace_back(function);
			}
        }
	}
    return file;
}
