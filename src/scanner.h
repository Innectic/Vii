
#include "vii.h"
#include <vector>
#include <fstream>
#include <iostream>

class Scanner {
private:
    std::string file;

public:
    Scanner();
    ~Scanner();

    std::vector<Token> tokenize(const std::string& file);
    void parse();  // TODO: This should return something real
};
