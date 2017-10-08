
#include "file_handler.h"
#include "c_converter.h"
#include <memory>

#include "workspace.h"
#include "token.h"
#include "scanner.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "   ERROR: Must supply at least one file to compile" << std::endl;
        return -1;
    }
    
    auto workspace = std::make_unique<WorkSpace>();
    auto fileHandler = std::make_unique<FileHandler>();
    auto converter = std::make_unique<C_Converter>();
    auto scanner = std::make_unique<Scanner>(*workspace.get());

    workspace->set_defaults();

    for (auto i = 1; i < argc; i++) {
        std::string current = argv[i];
        if (current == "") continue;
        std::string next = "";

        if (argv[i + 1]) next = argv[i + 1];

        if (current == "-w") {
            workspace->directory = fileHandler->getCurrentDirectory();
            workspace->original_files = fileHandler->getFilesInDirectory(true, workspace->directory);
        }
    }

    workspace->load_configuration();

    std::vector<Token> tokens = scanner->tokenize("test.vii");
    const SourceFile* file = scanner->parse(tokens);

    for (auto decl : file->decls) {
        std::cout << "NAME: " << decl.name << ", VALUE: " << decl.value << ", SCOPE: " << decl.scope << ", TYPE: " << token_map[decl.type] << std::endl;
    }

    for (auto func : file->function_calls) {
        std::cout << "NAME: " << func.function_name << std::endl;
        for (auto arg : func.arguments) {
            std::cout << "TYPE: " << token_map[arg.type] << ", VALUE: " << arg.value << std::endl;
        }
    }
    
    for (auto func : file->functions) {
        std::cout << "CUSTOM: " << func.function_name << std::endl;
        for (auto arg : func.arguments) {
            std::cout << "TYPE: " << token_map[arg.type] << ", VALUE: " << arg.value << std::endl;
        }
    }

    std::cout << "================" << std::endl;

    for (auto a : tokens) {
        std::cout << token_map[a.type] << ", " << a.value << std::endl;
    }


    delete file;

    return 0;
}
