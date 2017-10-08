
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

// TODO: Make this actually do something useful...
// This only supports things like 123, nothing like 12.3
//
const std::string Scanner::read_number() {
	std::string found = "";
	while (it < this->end) {
		if (!Util::is_number(*it)) break;
		found += *it;
		it++;
	}
	return found;
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
				auto taken = this->read_number();
				Token token = { TokenType::INT, taken };
				tokens.emplace_back(token);
				continue;
			}
			else {
				std::string a;
				a += *it;

				// Get the type of the character we're currently looking at
				auto type = get_token_type(a);

				// If it's an ident, that means it's not something we have.
				// So, add that to our currently found (for later use) string.
				if (type == TokenType::IDENTIFIER) {
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
					continue;
				}

				// Since we have something, lets check it out.
				auto next_token = *(it + 2);
				if (next_token.type == TokenType::ASSIGN) {
					// Since it's an assign, we need to do one more check - Make sure there's a value!
					if (!this->has_next(it + 2, tokens.end())) {
						// TODO: See the above TODO about error reporting
						std::cout << "Variable decleration has no value" << std::endl;
						continue;
					}
					// Since we have a value, we can turn this into a real variable!
					auto value_token = *(it + 3);
					Decleration decl = {
						0, 0, value_token.type, token.value, value_token.value, "cool_scope" // TODO: Real scopes
					};
					file->decls.emplace_back(decl);
				}
			}
			else if (the_next_token.type == TokenType::LPAREN) {
				// If we have a left paren, then we MUST be calling a function.
				// So, lets take everything until ) as arguments.

				// But before we can do that, we need to make sure we even *have* args to do that with.
				if (!this->has_next(it + 1, tokens.end())) {
					// TODO: See above TODO about error reporting.
					std::cout << "Function call doesn't have args / end" << std::endl;
					continue;
				}

				std::vector<Token> arguments;
				auto argument_it = (it + 2);

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