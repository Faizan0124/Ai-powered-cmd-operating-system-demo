#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

void init_fs();
std::vector<std::string> list_directory(const std::string& path);
std::string read_file(const std::string& path);
bool create_file(const std::string& path, const std::string& content);
bool delete_file(const std::string& path);
std::vector<std::string> search_files(const std::string& query);

#endif
