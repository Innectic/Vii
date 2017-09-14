
#include "parser.h"

Parser::Parser(bool debugMode) :
	debugMode(debugMode) {
}

Parser::~Parser() {

}

std::vector<SourceFile*> Parser::parseWorkspace(const WorkSpace& workspace) {
	std::vector<SourceFile*> parsed;
	for (std::string file : workspace.originalFiles) {
		auto sourceFile = this->parse(file);
		if (sourceFile == nullptr) {
			std::cout << "File to parse didn't exist?" << std::endl;
			continue;
		}
		parsed.push_back(sourceFile);
	}
	return parsed;
}

SourceFile* Parser::parse(const std::string& fileName) {
	std::unique_ptr<SourceFile> sourceFile = std::make_unique<SourceFile>();
	sourceFile->fileName = fileName;
	std::ifstream input;
	input.open(fileName);

	if (!input) return nullptr;

	std::string current = "";
	while (std::getline(input, current)) {
		sourceFile->lines++;
		current = Util::eatSpaces(current);
		// TODO: Literally the entire parser

		// Comment or blank, don't care.
		if (current.substr(0, 2) == "//" || current == "") {
			continue;
		}
	}
	input.close();
	return sourceFile.get();
}
