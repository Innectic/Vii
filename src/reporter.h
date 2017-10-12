#pragma once

#include <string>
#include <vector>
#include <iostream>

struct ViiError {
    std::string error;
    std::string file;
    int line;
    int column;
};

class Reporter {
public:
    const void report_error(const ViiError& error);
};
