
#include <iostream>
#include <string>
#include "parser.h"
#include "file_handler.h"
#include "c_converter.h"
#include <regex>
#include <memory>
#include "object.h"

void displayError(std::string error) {
	std::cout << "   ERROR: " << error << std::endl;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		displayError("Must supply at least one file to compile");
		std::cin.get();
		return 1;
	}
	
	std::unique_ptr<Workspace> workspace = std::make_unique<Workspace>();
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
			workspace->workspaceDirectory = FileHandler::getCurrentDirectory();
			workspace->files = FileHandler::getFilesInDirectory(true, workspace->workspaceDirectory);
		}
		else if (current == "-f") {
			workspace->usingWorkspace = false;
			workspace->workspaceDirectory = "";

			workspace->files.push_back(next);
		}
	}

	//std::unique_ptr<Parser> parser = std::make_unique<Parser>(true);
	//parser->parseWorkspace(*workspace);

	auto a = new Object<int>(1);
	std::cout << typeid(a->value).name() << std::endl;

	std::cout << std::endl;

	auto b = new Object<std::string>("a");
	std::cout << typeid(b->value).name() << std::endl;

	return 0;
}
