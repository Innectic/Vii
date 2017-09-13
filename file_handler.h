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
public:
	static inline std::vector<std::string> getFilesInDirectory(bool recurse, std::string directory) {
		using namespace std::experimental::filesystem;  // XXX: I hate `using`, do I want to keep this?
														// it makes the linux version of this work, so probably
		// TODO: Actually recurse
		std::vector<std::string> discoveredFiles;
		for (std::experimental::filesystem::directory_entry entry : directory_iterator(directory)) {
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

	static inline std::string getCurrentDirectory() {
		char currentPath[9999];  // TODO: This really shouldn't be a constant value set by us, it should come from the OS
		if (!GetCurrentDir(currentPath, sizeof(currentPath))) return "";
		return currentPath;
	}
};
