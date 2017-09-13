#pragma once

#include "symbol.h"
#include "source_file.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include "typer.h"

// TODO: This should be abstacted, so that there can be things like LLVM support.

class C_Converter {
private:
	std::string fileTemplate;
public:
	C_Converter();
	C_Converter(std::string& overrideTemplate);

	std::string typeToCType(const Type& type);
	void convert(const SourceFile& file);
	void emit(const std::string&, std::ofstream& stream);
};