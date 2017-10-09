
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

void C_Converter::emit_functions(const SourceFile& file, std::ofstream& stream) {
    for (auto function : file.functions) {
        std::string functionName = function.function_name; // TODO: things for main
        std::cout << functionName << std::endl;

        std::string returnType = this->type_to_c_type(function.return_type);
        std::string argumentString = "";

        for (auto arg : function.arguments) {
            std::string argType = this->type_to_c_type(arg.type);
            argumentString += argType + " " + arg.value;
        }

        this->emit(returnType + " " + functionName + "(" + argumentString +  ") {\n", stream);
        this->emit("std::cout << \"Hello\" << std::endl;\n", stream);
        this->emit("}\n", stream);
    }
}

void C_Converter::convert(const SourceFile& file) {
    std::ofstream fileStream;
    std::string fileName = std::regex_replace(file.fileName, std::regex("\\.vii"), ".cpp");
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
