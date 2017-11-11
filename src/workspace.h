#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>

#include "util.h"
#include "token.h"
#include "ast.h"
#include "reporter.h"
#include "typer.h"
#include "resolver.h"

enum class OptimizationLevel {
    NONE,
    LOW,
    MEDIUM,
    MAX,
    SIZE,
    SPEED
};

struct WorkSpace {
    Reporter reporter;
    Typer typer;
    Resolver resolver;

    inline WorkSpace(const Reporter& reporter, const Typer& typer, const Resolver& registry) :
        reporter(reporter), typer(typer), resolver(resolver) {

    }

    std::string directory;
    std::string comment_prefix;
    
    std::string block_comment_start;
    std::string block_comment_continue;
    std::string block_comment_end;

    std::vector<std::string> original_files;
    std::vector<AST_SourceFile*> source_files;

    OptimizationLevel optimization;

    void load_configuration();
    void set_defaults();

    bool had_error = false;

    const inline void report_error(const ViiError& error) {
        this->reporter.report_error(error);
        this->had_error = true;
    }
};
