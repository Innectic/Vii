
#include "vii.h"

#include "scanner.h"

int main(void) {
    auto scanner = new Scanner();

    auto tokens = scanner->tokenize("testing.vii");
    auto parsed = scanner->parse(tokens);

    delete scanner;

    For(tokens) {
        std::cout << ((int) it.type) << std::endl;
    }
    std::cin.get();

    For(parsed->stored) {
        std::cout << it.first << std::endl;

        For(it.second) {
            std::cout << "    - " << ((int) it.type) << std::endl;
        }
    }
    std::cin.get();

    return 0;
}
