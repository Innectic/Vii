#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "token.h"
#include "ast.h"

// TODO: This should be abstacted, so that there can be things like LLVM support.

class C_Converter {
private:
    std::string file_template;
public:
    C_Converter();
    C_Converter(std::string& override_template);

    const std::string type_to_c_type(const TokenType& type) const;
    void convert(const AST_SourceFile& file);
    void emit(const std::string&, std::ofstream& stream);

    void add_import(const std::string& lib, std::ofstream& stream);
    void emit_functions(const AST_SourceFile& file, std::ofstream& stream);
};
