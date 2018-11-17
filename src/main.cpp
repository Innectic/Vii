
#include "vii.h"

#include "scanner.h"

int main(void) {
    auto scanner = new Scanner();

    auto tokens = scanner->tokenize("testing.vii");
    auto parsed = scanner->parse(tokens, false);

    delete scanner;

    For(tokens) {
        std::cout << ((int) it.type) << std::endl;
    }
    std::cin.get();

    For(parsed) {
        std::cout << it << std::endl;
    }
    std::cin.get();

    return 0;
}
