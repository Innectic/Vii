
#include <iostream>
#include <string>
#include "parser.h"
#include "file_handler.h"
#include "c_converter.h"
#include <regex>
#include <memory>

#include "workspace.h"
#include "ir.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "   ERROR: Must supply at least one file to compile" << std::endl;
		std::cin.get();
		return 1;
	}
	
	std::unique_ptr<WorkSpace> workspace = std::make_unique<WorkSpace>();
	std::unique_ptr<FileHandler> fileHandler = std::make_unique<FileHandler>();

	std::unique_ptr<C_Converter> converter = std::make_unique<C_Converter>();

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

	// Create a fake sourcefile (For testing!)
	SourceFile* file = new SourceFile();

	file->fileName = "test.vii";
	file->lines = 20;

	Function* func = new Function();
	func->column = 1;
	func->line = 1;
	func->name = "magic";
	func->parentScope = "_";
	func->scope = ".magic";
	func->isMain = true;
	func->returnType = Type::NONE;

	Decleration* decl = new Decleration();
	decl->column = 1;
	decl->line = 1;
	decl->name = "test";
	decl->scope = "_";
	decl->type = Type::STRING;

	// func->arguments.push_back(*decl);
	file->functions.push_back(*func);

	converter->convert(*file);

	delete func;
	delete decl;
	delete file;

	return 0;
}
