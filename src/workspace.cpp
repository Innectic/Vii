
#include "workspace.h"

const std::vector<std::string> split_string(const std::string& splitting, const std::string& delim) {
    std::vector<std::string> matches;

    std::size_t current, previous = 0;
    current = splitting.find_first_of(delim);
    while (current != std::string::npos) {
        std::string matched = splitting.substr(previous, current - previous);
        if (matched != "" && matched != " ") matches.emplace_back(matched);
        previous = current + 1;
        current = splitting.find_first_of(delim, previous);
    }
    matches.emplace_back(splitting.substr(previous, current - previous));

    return matches;
}

void WorkSpace::set_defaults() {
    this->block_comment_start= "/*";
    this->block_comment_continue = "*";
    this->block_comment_end = "*/";

    this->comment_prefix = "//";
    this->optimization = OptimizationLevel::NONE;
}

void WorkSpace::load_configuration() {
    std::string fileName = this->directory + "/workspace.vii";

    std::vector<std::string> contents = Util::read_file_to_vector(this->directory + "/workspace.vii");
    if (contents.size() <= 0) return;

    for (auto& line : contents) {
        // Remove all spaces from the string. We don't REALLY need them.
        std::string::iterator end_pos = std::remove(line.begin(), line.end(), ' ');
        line.erase(end_pos, line.end());
        
        auto matches = split_string(line, ":=");        
        if (matches.size() < 2) {
            std::cout << "Configuration option " << matches[0] << " has no value!" << std::endl;
            continue;
        }

        std::string lhs = matches[0];
        std::string rhs = matches[1];
        
        if (lhs == "optimization") {
            if (rhs == "none") this->optimization = OptimizationLevel::NONE;
            else if (rhs == "low") this->optimization = OptimizationLevel::LOW;
            else if (rhs == "medium") this->optimization = OptimizationLevel::MEDIUM;
            else if (rhs == "max") this->optimization = OptimizationLevel::MAX;
            else if (rhs == "size") this->optimization = OptimizationLevel::SIZE;
            else if (rhs == "speed") this->optimization = OptimizationLevel::SPEED;
            else std::cout << "Invalid value for optimization level: " << rhs << std::endl;
        }
    }
}
