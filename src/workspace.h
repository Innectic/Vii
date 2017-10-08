#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include "util.h"

#include "token.h"

enum OptimizationLevel {
	OP_NONE, OP_LOW, OP_MEDIUM, OP_MAX, OP_SIZE, OP_SPEED
};

struct WorkSpace {
	std::string directory;

	std::string comment_prefix;
	
	std::string block_comment_start;
	std::string block_comment_continue;
	std::string block_comment_end;

	std::vector<std::string> original_files;
	std::vector<SourceFile> source_files;

	OptimizationLevel optimization;

	void load_configuration();
	void set_defaults();
};
