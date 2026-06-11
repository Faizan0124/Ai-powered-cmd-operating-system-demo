#include "filesystem.h"
#include "crypto_utils.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

const std::string VFS_ROOT = "VFS_Storage";
const std::string ENCRYPTION_KEY = "secure_os_key_123";

void init_fs() {
    if (!fs::exists(VFS_ROOT)) {
        fs::create_directory(VFS_ROOT);
    }
}

std::vector<std::string> list_directory(const std::string& path) {
    std::vector<std::string> items;
    std::string target = VFS_ROOT;
    if (!path.empty()) target += "/" + path;

    if (fs::exists(target) && fs::is_directory(target)) {
        for (const auto& entry : fs::directory_iterator(target)) {
            items.push_back(entry.path().filename().string());
        }
    }
    return items;
}

std::string read_file(const std::string& path) {
    std::string target = VFS_ROOT + "/" + path;
    if (!fs::exists(target)) return "";

    std::ifstream in(target, std::ios::binary);
    std::string encrypted_content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    
    return encrypt_decrypt(encrypted_content, ENCRYPTION_KEY);
}

bool create_file(const std::string& path, const std::string& content) {
    std::string target = VFS_ROOT + "/" + path;
    std::ofstream out(target, std::ios::binary);
    if (!out.is_open()) return false;

    std::string encrypted = encrypt_decrypt(content, ENCRYPTION_KEY);
    out.write(encrypted.data(), encrypted.size());
    return true;
}

bool delete_file(const std::string& path) {
    std::string target = VFS_ROOT + "/" + path;
    if (fs::exists(target)) {
        return fs::remove(target);
    }
    return false;
}

std::vector<std::string> search_files(const std::string& query) {
    std::vector<std::string> results;
    for (const auto& entry : fs::recursive_directory_iterator(VFS_ROOT)) {
        if (entry.path().filename().string().find(query) != std::string::npos) {
            results.push_back(entry.path().lexically_relative(VFS_ROOT).string());
        }
    }
    return results;
}
