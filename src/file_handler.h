#pragma once

#include <vector>
#include <string>
#include <experimental/filesystem>

#ifdef _WIN32
#include <stdio.h>
#include <direct.h>
#define get_current_dir _getcwd
#endif
#ifdef __linux__
#include <unistd.h>
#define get_current_dir getcwd
#endif

class FileHandler {
private:
    inline const std::vector<std::string> check_path(const std::experimental::filesystem::v1::directory_entry& entry) const {
        std::vector<std::string> discovered_files;
        for (auto& name : entry.path().filename()) {
            // We only want to take the `.vii` files.
            if (name.extension() == ".vii") {
                std::string whole_name = entry.path().parent_path().string();
                whole_name.append("/").append(entry.path().root_directory().string()).append(name.string());
				discovered_files.emplace_back(whole_name);
            }
        }
        return discovered_files;
    }

    inline const std::vector<std::string> recurse_files(const std::string& directory) const {
        using namespace std::experimental::filesystem;

        std::vector<std::string> discovered_files;
        for (directory_entry entry : recursive_directory_iterator(directory))
            for (auto& in : this->check_path(entry))
				discovered_files.emplace_back(in);
        return discovered_files;
    }

    inline const std::vector<std::string> get_in_directory(const std::string& directory) const {
        using namespace std::experimental::filesystem;

        std::vector<std::string> discovered_files;
        for (directory_entry entry : directory_iterator(directory))
            for (auto& in : this->check_path(entry))
				discovered_files.emplace_back(in);
        return discovered_files;
    }

public:
    const inline std::vector<std::string> get_files_in_directory(bool recurse, std::string directory) const {
        return recurse ? this->recurse_files(directory) : this->get_in_directory(directory);
    }

    const inline std::string get_current_directory() const {
        char current_path[9999];
        if (!get_current_dir(current_path, sizeof(current_path))) return "";
        return current_path;
    }
};
