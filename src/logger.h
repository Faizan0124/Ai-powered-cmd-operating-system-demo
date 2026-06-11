#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>

void log_action(const std::string& username, const std::string& action);
std::vector<std::string> get_admin_logs();

#endif
