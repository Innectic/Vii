#pragma once

#include <string>
#include <map>
#include <iostream>

#include "util.h"
#include "token.h"
#include "ast.h"
#include "workspace.h"
#include "reporter.h"

class Scanner {
private:
    std::string::iterator beg;
    std::string::iterator it;
    std::string::iterator end;

    std::string fileName;

    WorkSpace& workspace;

public:
    std::vector<std::string> usedNames;
    Scanner(WorkSpace& workspace);
    ~Scanner();

    const std::string read_string(const char& delim);
    const Token read_number();
    const bool check_comment();

    const bool can_use_name(std::string name);

    const std::vector<Token> tokenize(const std::string& filename);
    const AST_SourceFile* parse(std::vector<Token>& tokens);

    // TODO: #TooManyOverloads - This can probably just be templated, tbh

    const inline bool has_next() {
        return this->has_next(this->it, this->end);
    }

    const inline bool has_next(const std::string::iterator& it) {
        return this->has_next(it, this->end);
    }

    const inline bool has_next(const std::string::iterator& it, const std::string::iterator& end) {
        return it < end;
    }

    const inline bool has_next(const std::vector<Token>::iterator& it, const std::vector<Token>::iterator& end) {
        return it < end;
    }
};
