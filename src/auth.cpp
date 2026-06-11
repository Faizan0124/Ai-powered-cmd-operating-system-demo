#include "auth.h"
#include "crypto_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

const std::string DB_FILE = "users.txt";

void initialize_auth() {
    std::ifstream file(DB_FILE);
    if (!file.is_open()) {
        std::ofstream out(DB_FILE);
        out << "admin," << hash_password("admin123") << ",admin\n";
        out << "user," << hash_password("user123") << ",user\n";
    }
}

std::string authenticate(const std::string& username, const std::string& password) {
    std::ifstream file(DB_FILE);
    std::string line;
    std::string pwd_hash = hash_password(password);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string u, h, r;
        std::getline(ss, u, ',');
        std::getline(ss, h, ',');
        std::getline(ss, r, ',');

        if (u == username && h == pwd_hash) {
            return r;
        }
    }
    return "";
}

bool add_user(const std::string& current_role, const std::string& username, const std::string& password, const std::string& role) {
    if (current_role != "admin") {
        std::cout << "Error: Only admins can add users.\n";
        return false;
    }

    std::ofstream out(DB_FILE, std::ios::app);
    out << username << "," << hash_password(password) << "," << role << "\n";
    return true;
}
