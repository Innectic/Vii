
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

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "   ERROR: Must supply at least one file to compile" << std::endl;
		std::cin.get();
		return 1;
	}
	
	std::unique_ptr<Workspace> workspace = std::make_unique<Workspace>();
	std::unique_ptr<FileHandler> fileHandler = std::make_unique<FileHandler>();
	workspace->commentChar = "//"; // TODO: Pull from some-sort of a workspace conf

	for (auto i = 1; i < argc; i++) {
		std::string current = argv[i];
		if (current == "") continue;
		std::string next = "";

		if (argv[i + 1]) {
			next = argv[i + 1];
		}

		if (current == "-w") {
			workspace->usingWorkspace = true;
			workspace->workspaceDirectory = fileHandler->getCurrentDirectory();
			workspace->files = fileHandler->getFilesInDirectory(true, workspace->workspaceDirectory);
		}
		else if (current == "-f") {
			workspace->usingWorkspace = false;
			workspace->workspaceDirectory = "";

			workspace->files.push_back(next);
		}
	}

	//std::unique_ptr<Parser> parser = std::make_unique<Parser>(true);
	//parser->parseWorkspace(*workspace);

	return 0;
}
