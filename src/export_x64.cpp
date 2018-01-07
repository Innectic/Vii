
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

const std::string build_argument_string(AST_FunctionCall* function, bool include_type) {
	assert(function);

	std::string argument_string = "";

	for (auto arg = function->arguments.begin(); arg < function->arguments.end(); arg++) {
		if (include_type) {
			auto arg_type = convert_type(arg->type);
			argument_string += arg_type + " ";
		}
		std::string prefix = "";
		std::string suffix = "";
		if (!include_type) {
			if (arg->type == TokenType::STRING) {
				prefix = "\"";
				suffix = "\"";
			} else if (arg->type == TokenType::CHAR) {
				prefix = "'";
				suffix = "'";
			}
		}
		argument_string += prefix + arg->value + suffix;
		if (arg != function->arguments.end() - 1) argument_string += ", ";
	}
	return argument_string;
}

const void add_import(const std::string& lib, std::ofstream& stream) {
    std::string line = "#include " + lib + "\n";
    stream << line;
}

Export_x64::Export_x64(bool allow_native) : allow_native(allow_native) {}

const std::string Export_x64::name() const {
    return "x64";
}

const void handle_math_segment(AST_Math* math, std::ofstream& stream) {
	assert(math);

	for (auto it = math->operations.begin(); it < math->operations.end(); it++) {
		if (it->chain) stream << token_map[it->operation] << it->second_value;
		else stream << it->first_value << token_map[it->operation] << it->second_value;
	}
}

const void Export_x64::begin(const AST_SourceFile& source_file, std::ofstream& stream) {
	this->begin(source_file.contained, stream, source_file.mainFunction);
}

const void Export_x64::begin(const std::vector<AST_Type*> contained, std::ofstream& stream, AST_Function* main_function) {
	for (auto& potential : contained) {
		if (is_type<AST_Declaration*>(potential)) {
			auto decl = static_cast<AST_Declaration*>(potential);
			assert(decl);

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
			assert(builtin);

			std::string ready_line = allow_native ? native_map[builtin->type] : internal_map[builtin->type];
			ready_line = Util::replace(ready_line, "<CUSTOM>", build_argument_string(builtin, false));
			stream << ready_line + "\n";
		}
		else if (is_type<AST_FunctionCall*>(potential)) {
			// Are we defining a function?
			if (is_type<AST_Function*>(potential)) {
				auto function = static_cast<AST_Function*>(potential);
				assert(function);

				auto function_name = main_function && main_function == function ? "real_main" : function->name;
				
				auto return_type = convert_type(function->return_type);
				stream << return_type + " " + function_name + "(" + build_argument_string(function, true) + ") {\n";
				
				this->begin(function->contained, stream, nullptr);
				stream << "}\n";
				continue;
			}
			// Nope, so we're calling one.
			auto call = static_cast<AST_FunctionCall*>(potential);
			assert(call);

			if (call->native) {
				if (!this->allow_native) {
					std::cout << "Cannot define or call a native method unless this is the std lib." << std::endl;
					continue;
				}
				auto definition = get_native(call->name);
				stream << Util::replace(definition, "<CUSTOM>", build_argument_string(call, false)) << ";\n";
				continue;
			}
			stream << call->name << "(" << build_argument_string(call, false) << ");\n";
		}
		else if (is_type<AST_If_Block*>(potential)) {
			auto if_block = static_cast<AST_If_Block*>(potential);
			assert(if_block);

			// Export the main if
			stream << "if (" << if_block->first_block->conditional << ") {\n";
			this->begin(*if_block->first_block->contained, stream);
			stream << "}\n";
			// TODO: Else if / else
		}
	}
}

const void Export_x64::go(const std::string& destination_file, const AST_SourceFile& source_file) {
    std::ofstream stream;
    stream.open(destination_file);

	// Import all the things
	add_import("<string>", stream);
	add_import("<iostream>", stream);
	if (!allow_native) add_import("\"io.cpp\"", stream);

	// Export the AST tree to C++ code.
    this->begin(source_file, stream);
	if (!allow_native) stream << FILE_TEMPLATE;
}
