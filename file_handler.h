#pragma once

#include <vector>
#include <string>
#include <experimental/filesystem>

#ifdef _WIN32
#include <stdio.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#endif
#ifdef __linux__
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

class FileHandler {
private:
	// TODO: Lots of repeated code in here, generalize it.
	inline const std::vector<std::string> recurseFiles(const std::string& directory) const {
		using namespace std::experimental::filesystem;

		std::vector<std::string> discoveredFiles;
		for (directory_entry entry : recursive_directory_iterator(directory)) {
			for (auto name : entry.path().filename()) {
				// We only want to take the `.vii` files.
				if (name.extension() == ".vii") {
					std::string wholeName = entry.path().parent_path().string();
					wholeName.append(entry.path().root_directory().string()).append(name.string());
					discoveredFiles.push_back(wholeName);
				}
			}
		}
		return discoveredFiles;
	}

	inline const std::vector<std::string> getInDirectory(const std::string& directory) const {
		using namespace std::experimental::filesystem;

		std::vector<std::string> discoveredFiles;
		for (directory_entry entry : directory_iterator(directory)) {
			for (auto name : entry.path().filename()) {
				// We only want to take the `.vii` files.
				if (name.extension() == ".vii") {
					std::string wholeName = entry.path().parent_path().string();
					wholeName.append(entry.path().root_directory().string()).append(name.string());
					discoveredFiles.push_back(wholeName);
				}
			}
		}
		return discoveredFiles;
	}

public:
	const inline std::vector<std::string> getFilesInDirectory(bool recurse, std::string directory) const {
		return recurse ? this->recurseFiles(directory) : this->getInDirectory(directory);
	}

	const inline std::string getCurrentDirectory() const {
		char currentPath[9999];  // TODO: This really shouldn't be a constant value set by us, it should come from the OS
		if (!GetCurrentDir(currentPath, sizeof(currentPath))) return "";
		return currentPath;
	}
};
