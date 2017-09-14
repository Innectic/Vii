#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>

#include "source_file.h"

struct WorkSpace {
	std::string directory;

	std::string commentPrefix;
	
	std::string blockCommentStart;
	std::string blockCommentContinue;
	std::string blockCommentEnd;

	std::vector<std::string> originalFiles;
	std::vector<SourceFile> sourceFiles;

	void loadConfiguration();
	void setDefaults();
};
