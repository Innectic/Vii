
#include <memory>

#include "reporter.h"
#include "workspace.h"
#include "token.h"
#include "ast.h"
#include "scanner.h"
#include "file_handler.h"
#include "c_converter.h"


template<class First>
const inline bool is_type(AST_Type* t) {
    return dynamic_cast<First>(t) != nullptr;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "   ERROR: Must supply at least one file to compile" << std::endl;
        return -1;
    }

    auto reporter = std::make_unique<Reporter>();

    auto workspace = std::make_unique<WorkSpace>(*reporter.get());
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
    const AST_SourceFile* file = scanner->parse(tokens);

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
            std::cout << "This is a decl: " << decl->name << ", and this is my type: " << token_map[decl->type] << ", and if you're interested in my value: " << decl->value << std::endl;
        }
    }

    if (reporter->errors.size() > 0) {
        std::cout << "Encountered " << reporter->errors.size() << "errors. Will not build.";
    }
    else {
        //converter->convert(*file);
    }

    delete file;

    return 0;
}

