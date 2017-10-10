
#include "c_converter.h"

C_Converter::C_Converter() :
    file_template("int main(int argc, char* argv[]) { realMain(); }\n") {
}


C_Converter::C_Converter(std::string& overrideTemplate) :
    file_template(overrideTemplate) {
}

const std::string C_Converter::type_to_c_type(const TokenType& type) const {
    switch (type) {
    case TokenType::INT:
        return "int";
    case TokenType::STRING:
        return "std::string";
    default:
        return "void";
    }
}

void C_Converter::add_import(const std::string& lib, std::ofstream& stream) {
    std::string line = "#include " + lib + "\n";
    this->emit(line, stream);
}

void C_Converter::emit_functions(const AST_SourceFile& file, std::ofstream& stream) {
    for (auto potential : file.contained) {
        // We only want to do thing with functions
        if (!is_type<AST_Function*>(potential)) continue;
        auto function = static_cast<AST_Function*>(potential);
        // Check if this is the main function
        std::string functionName = function->name; // TODO: things for main
        if (file.mainFunction && file.mainFunction == function) functionName = "realMain";

        std::string returnType = this->type_to_c_type(function->return_type);
        std::string argumentString = "";

        for (auto arg : function->arguments) {
            std::string argType = this->type_to_c_type(arg.type);
            argumentString += argType + " " + arg.name;
        }

        this->emit(returnType + " " + functionName + "(" + argumentString +  ") {\n", stream);
        
        for (auto contained : function->contained) {
            // TODO: Builtin functions
            if (is_type<AST_Declaration*>(contained)) {
                auto decl = static_cast<AST_Declaration*>(contained);

                std::string type = this->type_to_c_type(decl->type);
                // TODO: this only supports strings right now...
                this->emit(type + " " + decl->name + " = \"" + decl->value + "\";\n", stream);
            }
            else if (is_type<AST_Builtin*>(contained)) {
                // TODO: Do something with a standard lib here, so we can actually
                // check the types being passed in.
                auto builtin = static_cast<AST_Builtin*>(contained);
                std::string ready_line = builtin_map[builtin->type];

                std::string arguments;
                for (auto arg : builtin->arguments) {
                    arguments += arg.name + " "; // HACK: this shouldn't put a space at the end if it's the last element
                }

                ready_line = Util::replace(ready_line, "<CUSTOM>", arguments);
                this->emit(ready_line + "\n", stream);
            }
        }

        this->emit("}\n", stream);
    }
}

void C_Converter::convert(const AST_SourceFile& file) {
    std::ofstream fileStream;
    std::string fileName = std::regex_replace(file.file_name, std::regex("\\.vii"), ".cpp");
    fileStream.open(fileName);

    // TODO: Only add imports when needed, and detect other imports that we need
    this->add_import("<string>", fileStream);
    this->add_import("<iostream>", fileStream);

    this->emit_functions(file, fileStream);
    
    this->emit(this->file_template, fileStream);

    fileStream.close();
}

void C_Converter::emit(const std::string& line, std::ofstream& stream) {
    stream << line;
}
