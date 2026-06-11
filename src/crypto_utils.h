#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <string>

std::string encrypt_decrypt(const std::string& input, const std::string& key);
std::string hash_password(const std::string& password);

#endif
