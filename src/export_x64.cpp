
#include "export_x64.h"

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

const void Export_x64::begin(const AST_SourceFile& source_file, std::ofstream& stream) {
    std::cout << std::endl;
    add_import("<string>", stream);
    add_import("<iostream>", stream);
    if (!allow_native) add_import("\"io.cpp\"", stream);

    for (auto potential : source_file.contained) {
        // We only want to do thing with functions
        if (!is_type<AST_Function*>(potential)) continue;
        auto function = static_cast<AST_Function*>(potential);
        // Check if this is the main function
        std::string functionName = function->name; // TODO: things for main
        if (source_file.mainFunction && source_file.mainFunction == function) functionName = "real_main";

        std::string returnType = convert_type(function->return_type);
        std::string argumentString = "";

        for (auto arg : function->arguments) {
            std::string argType = convert_type(arg.type);
            argumentString += argType + " " + arg.name;
        }

        stream << returnType + " " + functionName + "(" + argumentString + ") {\n";

        for (auto contained : function->contained) {
            std::cout << contained->my_name() << std::endl;
            if (is_type<AST_Declaration*>(contained)) {
                auto decl = static_cast<AST_Declaration*>(contained);

                std::string type = convert_type(decl->type);
                stream << type + " " + decl->name + " = ";

                switch (decl->type) {
                case TokenType::STRING:
                    stream << "\"" + decl->value + "\"";
                    break;
                case TokenType::INT:
                    stream << std::stoi(decl->value);
                    break;
                case TokenType::FLOAT:
                    stream << std::stof(decl->value);
                    break;
                default:
                    std::cout << "HOW DID AN INVALID TYPE GET TO THIS STAGE HOLY COW SOMETHING IS BROKEN! '" << token_map[decl->type] << "'" << std::endl;
                    break;
                }
                stream << ";\n";
            }
            else if (is_type<AST_Builtin*>(contained)) {
                // TODO: Do something with a standard lib here, so we can actually
                // check the types being passed in.
                auto builtin = static_cast<AST_Builtin*>(contained);
                std::string ready_line = allow_native ? native_map[builtin->type] : internal_map[builtin->type];

                std::string arguments;
                for (auto arg : builtin->arguments) {
                    std::string pre = "";;
                    std::string post = "";
                    if (arg.type == TokenType::STRING) {
                        pre += "\"";
                        post += "\" "; // HACK: this shouldn't put a space at the end if it's the last element
                    }
                    arguments += pre + arg.name + post;
                }


                ready_line = Util::replace(ready_line, "<CUSTOM>", arguments);
                stream << ready_line + "\n";
            }
            else if (is_type<AST_FunctionCall*>(contained)) { // TODO: Generalize this with the above one
                auto call = static_cast<AST_FunctionCall*>(contained);
                if (call->native) {
                    if (!this->allow_native) continue;
                    std::string ready_line = get_native(call->name);

                    std::string arguments;
                    for (auto arg : call->arguments) {
                        std::string pre = "";;
                        std::string post = "";
                        if (arg.type == TokenType::STRING) {
                            pre += "\"";
                            post += "\" "; // HACK: this shouldn't put a space at the end if it's the last element
                        }
                        arguments += pre + arg.name + post;
                    }

                    ready_line = Util::replace(ready_line, "<CUSTOM>", arguments);
                    stream << ready_line + "\n";
                }
            }
        }

        stream << "}\n";
    }

    if (!allow_native) stream << FILE_TEMPLATE;
}

const void Export_x64::go(const std::string& destination_file, const AST_SourceFile& source_file) {
    std::ofstream stream;
    stream.open(destination_file);

    this->begin(source_file, stream);
}