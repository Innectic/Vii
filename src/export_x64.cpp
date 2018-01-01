
#include "export_x64.h"
#include <cassert>

const std::string FILE_TEMPLATE = "int main(int argc, char* argv[]) { real_main(); } \n";

const std::string convert_type(const TokenType& type) {
    switch (type) {
    case TokenType::INT:
        return "int";
    case TokenType::STRING:
        return "std::string";
    case TokenType::CHAR:
        return "char";
    case TokenType::FLOAT:
        return "float";
    case TokenType::BOOL:
        return "bool";
    default:
        return "void";
    }
}

const void add_import(const std::string& lib, std::ofstream& stream) {
    std::string line = "#include " + lib + "\n";
    stream << line;
}

const std::string Export_x64::name() const {
    return "x64";
}

const void handle_math_segment(AST_Math* math, std::ofstream& stream) {
	assert(math);

    for (auto i = 0u; i < math->operations.size(); i++) {
        auto op = math->operations[i];
        auto last = i == math->operations.size() - 1;

        if (op.chain) stream << token_map[op.operation] << op.second_value;
        else stream << op.first_value << token_map[op.operation] << op.second_value;
    }
}

const void Export_x64::begin(const AST_SourceFile& source_file, std::ofstream& stream, bool should_import) {
	this->begin(source_file.contained, stream, should_import, source_file.mainFunction);
}

const void Export_x64::begin(const std::vector<AST_Type*> contained, std::ofstream& stream, bool should_import, AST_Function* main_function) {
	if (should_import) {
		add_import("<string>", stream);
		add_import("<iostream>", stream);
		if (!allow_native) add_import("\"io.cpp\"", stream);
	}

	for (auto& potential : contained) {
		if (is_type<AST_Function*>(potential)) {
			auto function = static_cast<AST_Function*>(potential);
			// Check if this is the main function
			std::string function_name = function->name;
			if (main_function && main_function == function) function_name = "real_main";

			std::string return_type = convert_type(function->return_type);
			std::string argument_string = "";

			for (auto arg = function->arguments.begin(); arg < function->arguments.end(); arg++) {
				std::string arg_type = convert_type(arg->type);
				argument_string += arg_type + " " + arg->value;
				if (arg != function->arguments.end() - 1) argument_string += ", ";
			}

			stream << return_type + " " + function_name + "(" + argument_string + ") {\n";
			this->begin(function->contained, stream, false, nullptr);
			stream << "}\n";
			continue;
		}
		else if (is_type<AST_Declaration*>(potential)) {
			auto decl = static_cast<AST_Declaration*>(potential);

			std::string type = decl->redecl ? "" : convert_type(decl->type);
			stream << type + " " + decl->name + " = ";

			switch (decl->type) {
			case TokenType::STRING:
				stream << "\"" + decl->value + "\"";
				break;
			case TokenType::INT:
				if (decl->math) handle_math_segment(decl->math, stream);
				else stream << std::stoi(decl->value);
				break;
			case TokenType::FLOAT:
				if (decl->math) handle_math_segment(decl->math, stream);
				else stream << std::stof(decl->value);
				break;
			case TokenType::CHAR:
				stream << "'" + decl->value + "'";
				break;
			case TokenType::BOOL:
				stream << decl->value;
				break;
			default:
				std::cout << "INTERNAL COMPILER ERROR: HOW DID AN INVALID TYPE GET TO THIS STAGE HOLY COW SOMETHING IS BROKEN! '" << token_map[decl->type] << "'" << std::endl;
				break;
			}
			stream << ";\n";
			stream << "}\n";
		} if (is_type<AST_Builtin*>(potential)) {
			auto builtin = static_cast<AST_Builtin*>(potential);
			std::string ready_line = allow_native ? native_map[builtin->type] : internal_map[builtin->type];

			// TODO: Generialization
			std::string arguments;
			for (auto arg = builtin->arguments.begin(); arg < builtin->arguments.end(); arg++) {
				std::string pre = "";;
				std::string post = "";
				if (arg->type == TokenType::STRING) {
					pre += "\"";
					post += "\"";
				}
				else if (arg->type == TokenType::CHAR) {
					pre += "'";
					post += "'";
				}
				else if (arg->type == TokenType::FLOAT) post += "F";
				if (arg != builtin->arguments.end() - 1) post += " , ";
				arguments += pre + arg->value + post;
			}

			ready_line = Util::replace(ready_line, "<CUSTOM>", arguments);
			stream << ready_line + "\n";
		}
		else if (is_type<AST_FunctionCall*>(potential)) {
			// TODO: Generalization
			auto call = static_cast<AST_FunctionCall*>(potential);
			if (call->native) {
				if (!this->allow_native) continue;
				std::string ready_line = get_native(call->name);

				std::string arguments;
				for (auto arg = call->arguments.begin(); arg < call->arguments.end(); arg++) {
					std::string pre = "";;
					std::string post = "";
					if (arg->type == TokenType::STRING) {
						pre += "\"";
						post += "\"";
					}
					else if (arg->type == TokenType::CHAR) {
						pre += "'";
						post += "'";
					}
					else if (arg->type == TokenType::FLOAT) post += "F";

					if (arg != call->arguments.end() - 1) post += " , ";
					arguments += pre + arg->value + post;
				}

				ready_line = Util::replace(ready_line, "<CUSTOM>", arguments);
				stream << ready_line + "\n";
			}
			else {
				// COPYPASTA from the above branch...
				std::string arguments;
				for (auto arg = call->arguments.begin(); arg < call->arguments.end(); arg++) {
					std::string pre = "";;
					std::string post = "";
					if (arg->type == TokenType::STRING) {
						pre += "\"";
						post += "\"";
					}
					else if (arg->type == TokenType::CHAR) {
						pre += "'";
						post += "'";
					}
					else if (arg->type == TokenType::FLOAT) post += "F";

					if (arg != call->arguments.end() - 1) post += " , ";
					arguments += pre + arg->value + post;
				}
				stream << call->name << "(" << arguments << ");\n";
			}
		}
		else if (is_type<AST_If_Block*>(potential)) {
			auto if_block = static_cast<AST_If_Block*>(potential);

			// Export the main if
			stream << "if (" << if_block->first_block->conditional << ") {\n";
			this->begin(*if_block->first_block->contained, stream, true);
			stream << "}\n";
			// TODO: Else if / else

		}
	}
	if (!allow_native && should_import) stream << FILE_TEMPLATE;
}

const void Export_x64::go(const std::string& destination_file, const AST_SourceFile& source_file) {
    std::ofstream stream;
    stream.open(destination_file);

    this->begin(source_file, stream, true);
}
