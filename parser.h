#pragma once

#include "source_file.h"
#include <vector>
#include <fstream>
#include "util.h"
#include <iostream>
#include "typer.h"
#include <memory>

struct Workspace {
	bool usingWorkspace;
	std::string workspaceDirectory;
	std::string commentChar;

	std::vector<std::string> files;
	std::vector<SourceFile*> sourceFiles;
};

class Parser {
private:
	bool debugMode;
public:
	Parser(bool debugMode);
	~Parser();

	std::vector<SourceFile*> parseWorkspace(const Workspace& workspace);
	SourceFile* parse(const std::string& fileName);
};
