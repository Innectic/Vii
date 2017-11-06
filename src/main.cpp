
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
        else if (current == "-stdc") {
            // This means we're compiling the standard lib.
            // So we don't want to do anything with the user's code.
            auto converter = std::make_unique<Export_x64>(true);
            auto files = fileHandler->getFilesInDirectory(true, "lib");

            std::map<std::string, std::vector<Token>> tokens;
            std::vector<AST_SourceFile*> parsed;

            auto start_time = Util::get_time();
            // Compile each file
            for (auto& file : files) tokens[file] = scanner->tokenize(file, true);
            auto end_time = Util::get_time();
            std::cout << "\033[1;36m> \033[0;32mFrontend time (s): " << ((double)(end_time - start_time) / 1000000000) << "\033[0m" << std::endl;
            
            start_time = Util::get_time();
            for (auto& entry : tokens) {
                const AST_SourceFile* f = scanner->parse(entry.second);
            
                auto name = Util::replace(entry.first, "lib/", "");
                name = Util::replace(name, ".vii", ".cpp");
                if (workspace->had_error) std::cout << "Encountered errors. Will not build." << std::endl;
                else converter->go(name, *f);
            }
            end_time = Util::get_time();
            std::cout << "\033[1;36m> \033[0;32mBackend time  (s): " << ((double)(end_time - start_time) / 1000000000) << "\033[0m" << std::endl;
            
            return 0;
        }
    }

    auto converter = std::make_unique<Export_x64>(false);
    workspace->load_configuration();

    auto start_time = Util::get_time();
    std::vector<Token> tokens = scanner->tokenize("test.vii", false);
    auto end_time = Util::get_time();
    std::cout << "\033[1;36m> \033[0;32mFrontend time (s): " << ((double) (end_time - start_time) / 1000000000) << "\033[0m" << std::endl;
    
    start_time = Util::get_time();
    const AST_SourceFile* file = scanner->parse(tokens);
    end_time = Util::get_time();
    std::cout << "\033[1;36m> \033[0;32mBackend time  (s): " << ((double) (end_time - start_time) / 1000000000) << "\033[0m" << std::endl;

    if (workspace->had_error) std::cout << "Encountered errors. Will not build.";
    else converter->go("test.cpp", *file);
    delete file;

    return 0;
}
