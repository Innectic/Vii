
#include "reporter.h"

const std::string GREEN = "\033[0;32m";
const std::string AQUA_BOLD = "\033[1;36m";
const std::string RED_BOLD = "\033[1;31m";
const std::string RESET = "\033[0m";

const void Reporter::report_error(const ViiError& error) {
    errors.emplace_back(error);

    std::cout << RED_BOLD << "Encountered an error: '" << error.error << "', in file " << error.file << " at line: " << error.line << " at col: " << error.column
        << RESET << std::endl;
}

const void Reporter::report_workspace(const int& lines, const int& comments) const {
    std::cout << std::endl;
    std::cout << AQUA_BOLD << "> " << GREEN << "Workspace statistics:" << std::endl;

    std::cout << GREEN << "  - Lines of code  : " << lines << RESET << std::endl;
    std::cout << GREEN << "  - Commented lines: " << comments << RESET << std::endl;
}
