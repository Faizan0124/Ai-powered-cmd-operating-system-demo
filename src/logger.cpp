#include "logger.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

const std::string LOG_FILE = "activity_logs.txt";

// Helper to get current timestamp
std::string current_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void log_action(const std::string& username, const std::string& action) {
    std::string safe_user = username.empty() ? "system" : username;
    
    // Write to a general log file
    std::ofstream general_out(LOG_FILE, std::ios::app);
    if (general_out.is_open()) {
        general_out << "[" << current_time() << "] " << safe_user << ": " << action << "\n";
    }
    
    // Also write to a specific user log file (e.g., admin_activity.txt, user_activity.txt)
    std::string user_log_file = safe_user + "_activity.txt";
    std::ofstream user_out(user_log_file, std::ios::app);
    if (user_out.is_open()) {
        user_out << "[" << current_time() << "] " << action << "\n";
    }
}

std::vector<std::string> get_admin_logs() {
    std::vector<std::string> logs;
    std::ifstream in(LOG_FILE);
    if (!in.is_open()) return logs;
    
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) {
            logs.push_back(line);
        }
    }
    return logs;
}
