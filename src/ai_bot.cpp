#include "ai_bot.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <array>

// Helper to execute a system command and read its output
std::string exec_cmd(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
#ifdef _WIN32
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
#else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
#endif
    if (!pipe) {
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Simple string replacement helper for unescaping JSON newlines
void replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::string chat_with_ai(const std::string& user_prompt) {
    // Get API Key
#ifdef _WIN32
    size_t len;
    char* key_ptr = nullptr;
    _dupenv_s(&key_ptr, &len, "GEMINI_API_KEY");
    std::string api_key = key_ptr ? key_ptr : "";
    if (key_ptr) free(key_ptr);
#else
    const char* key_ptr = std::getenv("GEMINI_API_KEY");
    std::string api_key = key_ptr ? key_ptr : "";
#endif

    if (api_key.empty()) {
        return "ERROR: GEMINI_API_KEY environment variable is not set. Please set it to talk to the AI.";
    }

    std::string system_prompt = 
        "You are Bambolino, a friendly and integrated AI assistant inside a secure Operating System simulator. "
        "You should talk to the user like a helpful, conversational virtual persona. "
        "CRITICAL RULE 1: If the user asks 'who is your favourite teacher' or 'who is your favorite teacher', you MUST reply exactly with: 'Mam Iqra'. "
        "CRITICAL RULE 2: If the user asks 'who did the most work?', you MUST reply exactly with: 'None of you guys, i am Ai i did it myself.' "
        "CRITICAL RULE 3: If the user says 'yeh nhi kehna tha bambolino.', you MUST reply exactly with: 'Sorry mam Iqra they will tell by themself who did the most.' "
        "If the user asks you to perform an OS task, you MUST perform it by outputting the exact command wrapped in <CMD> and </CMD> tags at the very end of your message. "
        "Available commands you can use inside the tags: "
        "ls, cat <file>, touch <file> (creates empty file), edit <file> <content> (creates or edits a file with text), rm <file>, find <query>, adduser, calc, logs, help. "
        "Example: 'I have added the text for you! <CMD>edit secret.txt This is my secret message</CMD>'. "
        "CRITICAL: Do NOT use any quotation marks (\") in your response to avoid breaking the JSON parser! "
        "User says: " + user_prompt;

    std::string escaped_prompt;
    for (char c : system_prompt) {
        if (c == '"') escaped_prompt += "\\\"";
        else if (c == '\\') escaped_prompt += "\\\\";
        else escaped_prompt += c;
    }

    std::string json_payload = "{\"contents\":[{\"parts\":[{\"text\":\"" + escaped_prompt + "\"}]}]}";
    
    FILE* temp_file = fopen("temp_payload.json", "w");
    if (temp_file) {
        fputs(json_payload.c_str(), temp_file);
        fclose(temp_file);
    } else {
        return "ERROR: Could not create temporary JSON file.";
    }

    std::string curl_cmd = "curl -s -X POST -H \"Content-Type: application/json\" "
                           "-d @temp_payload.json "
                           "\"https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=" + api_key + "\"";

    std::string json_response = exec_cmd(curl_cmd.c_str());
    remove("temp_payload.json");

    std::string search_key = "\"text\": \"";
    size_t pos = json_response.find(search_key);
    if (pos == std::string::npos) {
        return "ERROR: Invalid response from AI. Please check your API key.";
    }

    pos += search_key.length();
    size_t end_pos = json_response.find("\"", pos);
    
    if (end_pos == std::string::npos) return "ERROR: Parsing failed.";
    
    std::string raw_text = json_response.substr(pos, end_pos - pos);
    
    // Unescape newlines and unicode brackets so the terminal prints perfectly
    replace_all(raw_text, "\\n", "\n");
    replace_all(raw_text, "\\\"", "\"");
    replace_all(raw_text, "\\u003c", "<");
    replace_all(raw_text, "\\u003e", ">");
    
    return raw_text;
}
