#ifndef AI_BOT_H
#define AI_BOT_H

#include <string>

// Calls Gemini API for a conversational response, and optionally returns a command tag
std::string chat_with_ai(const std::string& user_prompt);

#endif
