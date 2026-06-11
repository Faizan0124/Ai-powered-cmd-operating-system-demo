#ifndef AUTH_H
#define AUTH_H

#include <string>

void initialize_auth();
std::string authenticate(const std::string& username, const std::string& password);
bool add_user(const std::string& current_role, const std::string& username, const std::string& password, const std::string& role);

#endif
