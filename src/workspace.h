#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include "util.h"

#include "ir.h"

enum OptimizationLevel {
	OP_NONE, OP_LOW, OP_MEDIUM, OP_MAX, OP_SIZE, OP_SPEED
};

struct WorkSpace {
	std::string directory;

	std::string commentPrefix;
	
	std::string blockCommentStart;
	std::string blockCommentContinue;
	std::string blockCommentEnd;

	std::vector<std::string> originalFiles;
	std::vector<SourceFile> sourceFiles;

	OptimizationLevel optimization;

	void loadConfiguration();
	void setDefaults();
};
