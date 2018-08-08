
#include "vii.h"

#include "scanner.h"

int main(void) {
    auto scanner = new Scanner();

    scanner->tokenize("testing.vii");

    delete scanner;

    return 0;
}
