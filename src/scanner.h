#pragma once

#include <string>
#include <map>
#include <iostream>
#include <chrono>

#include "util.h"
#include "token.h"
#include "workspace.h"

class Scanner {
private:
    std::string::iterator beg;
    std::string::iterator it;
    std::string::iterator end;

    WorkSpace& workspace;

    std::vector<std::string> usedNames;
public:
    Scanner(WorkSpace& workspace);
    ~Scanner();

    const std::string read_string();
    const std::string read_number();
    const bool check_comment();

    const bool can_use_name(std::string name);

    const std::vector<Token> tokenize(const std::string& filename);
    const SourceFile* parse(std::vector<Token>& tokens);

    // TODO: #TooManyOverloads - This can probably just be templated, tbh

    const inline bool has_next() {
        return this->has_next(this->it, this->end);
    }

    const inline bool has_next(std::string::iterator& it) {
        return this->has_next(it, this->end);
    }

    const inline bool has_next(std::string::iterator& it, std::string::iterator& end) {
        return it < end;
    }

    const inline bool has_next(std::vector<Token>::iterator& it, std::vector<Token>::iterator& end) {
        return it < end;
    }
};
