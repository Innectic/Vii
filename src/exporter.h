#pragma once

#include <string>
#include "ast.h"

class Exporter {
private:
    AST_SourceFile source_file;
public:
    virtual const std::string name() const = 0;

	const virtual void begin(const AST_SourceFile& source_file, std::ofstream& stream, bool should_import) = 0;
	const virtual void begin(const std::vector<AST_Type*> contained, std::ofstream& stream, bool should_import, AST_Function* main_function) = 0;
    const virtual void go(const std::string& destination_file, const AST_SourceFile& source_file) = 0;
};
