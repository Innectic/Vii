
#include <iostream>
#include <string>
#include "parser.h"
#include "file_handler.h"
#include "c_converter.h"
#include <regex>
#include <memory>

#include "object.h"
#include "object_int.h"
#include "object_string.h"
#include "workspace.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "   ERROR: Must supply at least one file to compile" << std::endl;
		std::cin.get();
		return 1;
	}
	
	std::unique_ptr<WorkSpace> workspace = std::make_unique<WorkSpace>();
	std::unique_ptr<FileHandler> fileHandler = std::make_unique<FileHandler>();

	for (auto i = 1; i < argc; i++) {
		std::string current = argv[i];
		if (current == "") continue;
		std::string next = "";

		if (argv[i + 1]) {
			next = argv[i + 1];
		}

		if (current == "-w") {
			workspace->directory = fileHandler->getCurrentDirectory();
			workspace->originalFiles = fileHandler->getFilesInDirectory(true, workspace->directory);
		}
	}

	//std::unique_ptr<Parser> parser = std::make_unique<Parser>(true);
	//parser->parseWorkspace(*workspace);

	workspace->loadConfiguration();

	return 0;
}
