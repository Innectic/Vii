
#include "vii.h"
#include <vector>
#include <fstream>
#include <iostream>

class Scanner {
private:
    std::string file;
    std::vector<ASTType*> types;

public:
    Scanner();
    ~Scanner();

    std::vector<Token> tokenize(const std::string& file);
    std::vector<ASTType*> parse(const std::vector<Token>& tokens, bool within_another_scope);
};
