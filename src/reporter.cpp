
#include "reporter.h"

const std::string GREEN = "\033[0;32m";
const std::string AQUA_BOLD = "\033[1;36m";
const std::string RED_BOLD = "\033[1;31m";
const std::string RESET = "\033[0m";

const void Reporter::report_error(const ViiError& error) {
    std::cout << RED_BOLD << "Encountered an error: '" << error.error << "', in file " << error.file << " at line: " << error.line << " at col: " << error.column
        << RESET << std::endl;
}
