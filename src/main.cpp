
#include "vii.h"

#include "scanner.h"

int main(void) {
    auto scanner = new Scanner();

    auto tokens = scanner->tokenize("testing.vii");
    auto parsed = scanner->parse(tokens);

    delete scanner;

    return 0;
}
