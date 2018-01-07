
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
#include "verification.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "   ERROR: Must supply at least one file to compile" << std::endl;
        return -1;
    }

    auto typer = std::make_unique<Typer>();
    auto resolver = std::make_unique<Resolver>(*typer.get());
    auto reporter = std::make_unique<Reporter>();
    auto workspace = std::make_unique<WorkSpace>(*reporter.get(), *typer.get(), *resolver.get());
    auto file_handler = std::make_unique<FileHandler>();
    auto scanner = std::make_unique<Scanner>(*workspace.get());
    auto verification = std::make_unique<Verification>(*workspace.get());
    
    workspace->set_defaults();

    for (auto i = 1; i < argc; i++) {
        std::string current = argv[i];
        if (current == "") continue;
        std::string next = "";

        if (argv[i + 1]) next = argv[i + 1];

        if (current == "-w") {
            workspace->directory = file_handler->get_current_directory();
            workspace->original_files = file_handler->get_files_in_directory(true, workspace->directory);
        }
        else if (current == "-stdc") {
            // This means we're compiling the standard lib.
            // So we don't want to do anything with the user's code.
            auto converter = std::make_unique<Export_x64>(true);
            auto files = file_handler->get_files_in_directory(true, "src/lib");

            std::map<std::string, std::vector<Token>> tokens;
            std::vector<AST_SourceFile*> parsed;

            auto start_time = Util::get_time();
            // Compile each file
            for (auto& file : files) tokens[file] = scanner->tokenize(file, true);
            auto end_time = Util::get_time();
            std::cout << "\033[1;36m> \033[0;32mFrontend time (s): " << ((double) (end_time - start_time) / 1000000000) << "\033[0m" << std::endl;
            
            start_time = Util::get_time();
            for (auto& entry : tokens) {
                const AST_SourceFile* f = scanner->parse(entry.second, true);
            
                auto name = Util::replace(entry.first, "lib/", "");
                name = Util::replace(name, ".vii", ".cpp");
                if (workspace->had_error) std::cout << "Encountered errors. Will not build." << std::endl;
                else converter->go(name, *f);
            }
            end_time = Util::get_time();
            std::cout << "\033[1;36m> \033[0;32mBackend time  (s): " << ((double) (end_time - start_time) / 1000000000) << "\033[0m" << std::endl;
            
            return 0;
        }
    }

    auto converter = std::make_unique<Export_x64>(false);
    workspace->load_configuration();

    auto total_time = Util::get_time();

    auto start_time = Util::get_time();
    std::vector<Token> tokens = scanner->tokenize("test.vii", false);
    auto end_time = Util::get_time() - start_time;
    
    start_time = Util::get_time();
    const AST_SourceFile* file = scanner->parse(tokens, true);
    auto end_parse = Util::get_time() - start_time;

    if (workspace->had_error) std::cout << "Encountered errors. Will not build.";
    else {
        start_time = Util::get_time();
        auto issues = verification->validate(file->contained);
        if (issues) return 1;
        converter->go("test.cpp", *file);
        auto conversion_time = Util::get_time() - start_time;

        start_time = Util::get_time();

        #ifdef _WIN32
        auto command = "compile_windows.bat " + workspace->directory + " test.cpp";
        system(command.c_str());
        #endif
        #ifdef __linux__
        #endif
        auto end_compile = Util::get_time() - start_time;

        auto total_end = Util::get_time();
        std::cout << "\033[1;36m> \033[0;32mFrontend time (s)     : " << ((double) end_time / 1000000000) << "\033[0m" << std::endl;
        std::cout << "\033[1;36m> \033[0;32mTotal backend time (s): " << ((double) (total_end - total_time) / 1000000000) << "\033[0m" << std::endl;
        std::cout << "  \033[1;36m> \033[0;32mInternal time (s)   : " << ((double) conversion_time / 1000000000) << "\033[0m" << std::endl;
        std::cout << "  \033[1;36m> \033[0;32mC++ time  (s)       : " << ((double) end_compile / 1000000000) << "\033[0m" << std::endl;
    }
    delete file;

    return 0;
}
