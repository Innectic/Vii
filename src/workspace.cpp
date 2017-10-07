
#include "workspace.h"

const std::vector<std::string> splitString(const std::string& splitting, const std::string& delim) {
	std::vector<std::string> matches;

	std::size_t current, previous = 0;
	current = splitting.find_first_of(delim);
	while (current != std::string::npos) {
		std::string matched = splitting.substr(previous, current - previous);
		matches.emplace_back(matched);
		previous = current + 1;
		current = splitting.find_first_of(delim, previous);
	}
	matches.emplace_back(splitting.substr(previous, current - previous));

	// TODO: Why can't this be in the main loop?
	// It seems to make the entire function hang and never exit, for some reason.
	std::vector<std::string> clean;
	for (auto dirty : matches) {
		if (dirty == " " || dirty == "") continue;
		clean.emplace_back(dirty);
	}

	return clean;
}

void WorkSpace::setDefaults() {
	// TODO: These are all temp values
	this->blockCommentStart = "/*";
	this->blockCommentContinue = "*";
	this->blockCommentEnd = "*/";

	this->commentPrefix = "//";
	this->optimization = OptimizationLevel::OP_NONE;
}

void WorkSpace::loadConfiguration() {
	std::string fileName = this->directory + "/workspace.vii";  // TODO: Maybe this file should be set by a compiler directive. #config ?

	std::vector<std::string> contents = Util::readFileToVector(this->directory + "/workspace.vii");
	if (contents.size() <= 0) return;

	for (auto line : contents) {
		// Remove all spaces from the string. We don't REALLY need them.
		std::string::iterator end_pos = std::remove(line.begin(), line.end(), ' ');
		line.erase(end_pos, line.end());
		
		auto matches = splitString(line, ":=");		
		if (matches.size() < 2) {
			std::cout << "Configuration option " << matches[0] << " has no value!" << std::endl;
			continue;
		}

		// TODO: lhs should be lowercased
		std::string lhs = matches[0];
		std::string rhs = matches[1];
		
		if (lhs == "optimization") {
			if (rhs == "none") this->optimization = OptimizationLevel::OP_NONE;
			else if (rhs == "low") this->optimization = OptimizationLevel::OP_LOW;
			else if (rhs == "medium") this->optimization = OptimizationLevel::OP_MEDIUM;
			else if (rhs == "max") this->optimization = OptimizationLevel::OP_MAX;
			else if (rhs == "size") this->optimization = OptimizationLevel::OP_SIZE;
			else if (rhs == "speed") this->optimization = OptimizationLevel::OP_SPEED;
			else std::cout << "Invalid value for optimization level: " << rhs << std::endl;
		}
	}
}
