
#include "export_x64.h"

const std::string FILE_TEMPLATE = "int main(int argc, char* argv[]) { real_main(); } \n";

std::vector<std::string> imported;
std::vector<std::string> header;
std::vector<std::string> body;

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
    for (auto exists : imported) if (exists == lib) return;
    imported.emplace_back(lib);
    
    std::string line = "#include " + lib + "\n";
    header.emplace_back(line);
}

const std::string Export_x64::name() const {
    return "x64";
}

const void Export_x64::begin(const AST_SourceFile& source_file) {
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

        body.emplace_back(returnType + " " + functionName + "(" + argumentString + ") {\n");

        for (auto contained : function->contained) {
            // TODO: Builtin functions
            if (is_type<AST_Declaration*>(contained)) {
                auto decl = static_cast<AST_Declaration*>(contained);
                if (decl->type == TokenType::STRING) add_import("<string>", stream);

                std::string type = convert_type(decl->type);
                // TODO: this only supports strings right now...
                body.emplace_back(type + " " + decl->name + " = \"" + decl->value + "\";\n");
            }
            else if (is_type<AST_Builtin*>(contained)) {
                // TODO: Do something with a standard lib here, so we can actually
                // check the types being passed in.
                auto builtin = static_cast<AST_Builtin*>(contained);
                if (builtin->type == BuiltinType::PRINT) add_import("<iostream>", stream);
                std::string ready_line = builtin_map[builtin->type];

                std::string arguments;
                for (auto arg : builtin->arguments) {
                    arguments += arg.name + " "; // HACK: this shouldn't put a space at the end if it's the last element
                }

                ready_line = Util::replace(ready_line, "<CUSTOM>", arguments);
                body.emplace_back(ready_line + "\n");
            }
        }

        body.emplace_back("}\n");
    }

    body.emplace_back(FILE_TEMPLATE);
}

const void Export_x64::go(const std::string& destination_file, const AST_SourceFile& source_file) {
    std::ofstream stream;
    stream.open(destination_file);

    this->begin(source_file);

    for (auto line : header) stream << line;
    for (auto line : body) stream << line;
}
