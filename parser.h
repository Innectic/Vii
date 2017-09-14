#pragma once

#include <vector>
#include <fstream>
#include "util.h"
#include <iostream>
#include "typer.h"
#include <memory>
#include "workspace.h"

class Parser {
private:
	bool debugMode;
public:
	Parser(bool debugMode);
	~Parser();

	std::vector<SourceFile*> parseWorkspace(const WorkSpace& workspace);
	SourceFile* parse(const std::string& fileName);
};
