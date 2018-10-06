
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
    ASTProject* parse(const std::vector<Token>& tokens);  // TODO: This should return something real
};
