
#include "workspace.h"

const std::vector<std::string> splitString(const std::string& splitting, const std::string& delim) {
	std::vector<std::string> matches;

	std::size_t current, previous = 0;
	current = splitting.find_first_of(delim);
	while (current != std::string::npos) {
		matches.push_back(splitting.substr(previous, current - previous));
		previous = current + 1;
		current = splitting.find_first_of(delim, previous);
	}
	matches.push_back(splitting.substr(previous, current - previous));
	return matches;
}

void WorkSpace::setDefaults() {
	// TODO: These are all temp values
	this->blockCommentStart = "/-";
	this->blockCommentContinue = "-";
	this->blockCommentEnd = "-/";

	this->commentPrefix = "//";
}

void WorkSpace::loadConfiguration() {
	std::string fileName = this->directory + "/workspace.vii";  // TODO: Maybe this file should be set by a compiler directive. #config ?
	std::ifstream input;
	input.open(fileName);

	if (!input) {
		this->setDefaults();
		return;
	}

	std::string current = "";
	while (std::getline(input, current)) {
		// Remove all spaces from the string. We don't REALLY need them.
		std::string::iterator end_pos = std::remove(current.begin(), current.end(), ' ');
		current.erase(end_pos, current.end());

		auto dirtyMatches = splitString(current, ":=");
		std::vector<std::string> matches;
		// TODO: /shrug why is this a thing
		for (auto dirty : dirtyMatches) {
			if (dirty == "" || dirty == " ") continue;
			matches.push_back(dirty);
		}

		if (matches.size() < 2) {
			std::cout << "Configuration option " << matches[0] << " has no value!" << std::endl;
			continue;
		}

		// TODO: lhs should be lowercased
		std::string lhs = matches[0];
		std::string rhs = matches[1];

		if (lhs == "commentPrefix") {
			this->commentPrefix = rhs;
		}
	}
	input.close();
}
