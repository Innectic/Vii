
#include <memory>

#include "workspace.h"
#include "reporter.h"

#include "token.h"
#include "ast.h"
#include "scanner.h"
#include "typer.h"
#include "file_handler.h"
#include "util.h"
#include "export_x64.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "   ERROR: Must supply at least one file to compile" << std::endl;
        return -1;
    }

    auto typer = std::make_unique<Typer>();
    auto reporter = std::make_unique<Reporter>();
    auto workspace = std::make_unique<WorkSpace>(*reporter.get(), *typer.get());
    auto fileHandler = std::make_unique<FileHandler>();
    auto converter = std::make_unique<Export_x64>();
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

    auto start_time = Util::get_time();
    std::vector<Token> tokens = scanner->tokenize("test.vii");
    auto end_time = Util::get_time();
    std::cout << "\033[1;36m> \033[0;32mFrontend time (s): " << ((double)(end_time - start_time) / 1000000000) << "\033[0m" << std::endl;
    
    start_time = Util::get_time();
    const AST_SourceFile* file = scanner->parse(tokens);
    end_time = Util::get_time();
    std::cout << "\033[1;36m> \033[0;32mBackend time  (s): " << ((double)(end_time- start_time) / 1000000000) << "\033[0m" << std::endl;

    for (auto entry : file->contained) {
        if (is_type<AST_FunctionCall*>(entry)) {
            auto f = static_cast<AST_FunctionCall*>(entry);
            std::cout << "This is a function: " << f->name << ", and these are my arguments: ";
            for (auto arg : f->arguments) {
                std::cout << arg.name << ", " << token_map[arg.type] << std::endl;
            }
        }
        else if (is_type<AST_Declaration*>(entry)) {
            auto decl = static_cast<AST_Declaration*>(entry);
            std::cout << "This is a decl: " << decl->name << ", and this is my type: " << token_map[decl->type] << ", and if you're interested in my value: " <<
                decl->value << ", and I live in: " << decl->scope << std::endl;
        }
    }

    if (workspace->had_error) std::cout << "Encountered errors. Will not build.";
    else {
        converter->go("test.cpp", *file);
    }
    delete file;

    return 0;
}
