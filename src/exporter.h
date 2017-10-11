#pragma once

#include <string>
#include "ast.h"

class Exporter {
private:
    AST_SourceFile source_file;
public:
    virtual const std::string name() const = 0;

    virtual const void begin(const std::string& destination_file, const std::string& destination) = 0;
    virtual const void go() = 0;
    virtual const void convert_functions() = 0;
};
