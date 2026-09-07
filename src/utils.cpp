#include "utils.h"
#include <filesystem>
#include <vector>
#include <string>

Texture2D LoadTextureSafe(const char* path) {
    if (std::filesystem::exists(path)) {
        return LoadTexture(path);
    }
    return {0};
}

std::vector<std::string> GetDirectoryFiles(const char* dirPath) {
    std::vector<std::string> files;
    if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
    }
    return files;
}

bool DirectoryExists(const char* dirPath) {
    return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
}