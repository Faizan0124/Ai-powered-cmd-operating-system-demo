#include "auth.h"
#include "filesystem.h"
#include "logger.h"
#include "colors.h"
#include "ai_bot.h"
#include <iostream>
#include <string>
#include <sstream>

#include <thread>
#include <chrono>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <mmsystem.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

std::string wrap_text(const std::string& text, size_t max_width = 80) {
    std::istringstream iss(text);
    std::string line;
    std::string result;

    while (std::getline(iss, line)) {
        size_t current_len = 0;
        std::istringstream words(line);
        std::string word;
        while (words >> word) {
            if (current_len + word.length() > max_width) {
                result += "\n  ";
                current_len = 2;
            }
            result += word + " ";
            current_len += word.length() + 1;
        }
        result += "\n";
    }
    
    // Remove the final trailing newline if it exists
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
}

void print_help() {
    std::cout << BOLDCYAN << "Available commands:\n" << RESET;
    std::cout << "  ls          - List files in current directory\n";
    std::cout << "  cat <file>     - Read file content\n";
    std::cout << "  touch <file>   - Create new empty file\n";
    std::cout << "  edit <file> <c>- Create or edit a file with text content\n";
    std::cout << "  rm <file>      - Delete file\n";
    std::cout << "  find <q>       - Search for files\n";
    std::cout << "  calc           - Simple calculator\n";
    std::cout << "  fsinfo         - Show Virtual File System statistics\n";
    std::cout << "  mem            - Show OS Memory Usage\n";
    std::cout << "  ps             - List running processes\n";
    std::cout << "  spawn <app>    - Start a new process (e.g. notepad)\n";
    std::cout << "  kill <pid>     - Terminate a process\n";
    std::cout << "  bgjob          - Run a background thread\n";
    std::cout << "  syscall        - Demonstrate low-level OS system calls\n";
    std::cout << "  showcase       - Print OS Lab Concepts summary\n";
    std::cout << "  adduser        - Add a new user (Admin only)\n";
    std::cout << "  logs           - View admin activity logs (Admin only)\n";
    std::cout << "  bambolino <m>  - Talk to Bambolino to perform tasks (e.g., 'bambolino make a file')\n";
    std::cout << "  help           - Show this help\n";
    std::cout << "  logout         - Logout and exit\n";
}

void calculator() {
    std::cout << BOLDYELLOW << "Enter expression (e.g. 5 + 3) or type 'q' to cancel: " << RESET;
    std::string line;
    std::getline(std::cin, line);
    
    // If the user just hits Enter or types 'q', back out cleanly
    if (line.empty() || line == "q" || line == "exit") {
        std::cout << YELLOW << "Calculator cancelled.\n" << RESET;
        return;
    }

    std::stringstream ss(line);
    double a, b;
    char op;
    
    if (ss >> a >> op >> b) {
        if (op == '+') std::cout << BOLDGREEN << "Result: " << a + b << RESET << "\n";
        else if (op == '-') std::cout << BOLDGREEN << "Result: " << a - b << RESET << "\n";
        else if (op == '*') std::cout << BOLDGREEN << "Result: " << a * b << RESET << "\n";
        else if (op == '/') {
            if (b != 0) std::cout << BOLDGREEN << "Result: " << a / b << RESET << "\n";
            else std::cout << BOLDRED << "Error: Division by zero\n" << RESET;
        } else {
            std::cout << BOLDRED << "Unknown operator.\n" << RESET;
        }
    } else {
        std::cout << BOLDRED << "Invalid input.\n" << RESET;
    }
}

// Global state for simplicity
std::string current_user = "";
std::string current_role = "";
bool running = true;

void process_command(std::string line) {
    if (line.empty()) return;
    std::stringstream ss(line);
    std::string cmd;
    ss >> cmd;

    if (cmd == "ls") {
        auto items = list_directory("");
        for (const auto& item : items) {
            std::cout << CYAN << item << RESET << "  ";
        }
        std::cout << "\n";
        log_action(current_user, "Executed ls command");
    } else if (cmd == "cat") {
        std::string file;
        ss >> file;
        if (!file.empty()) {
            std::cout << read_file(file) << "\n";
            log_action(current_user, "Read file: " + file);
        } else {
            std::cout << YELLOW << "Usage: cat <file>\n" << RESET;
        }
    } else if (cmd == "touch") {
        std::string file, content;
        ss >> file;
        if (!file.empty()) {
            std::cout << "Enter content (single line): ";
            std::getline(std::cin, content);
            create_file(file, content);
            std::cout << GREEN << "File created.\n" << RESET;
            log_action(current_user, "Created file: " + file);
        } else {
            std::cout << YELLOW << "Usage: touch <file>\n" << RESET;
        }
    } else if (cmd == "edit") {
        std::string file;
        ss >> file;
        if (!file.empty()) {
            std::string content;
            std::getline(ss, content);
            if (!content.empty() && content[0] == ' ') content = content.substr(1);
            create_file(file, content);
            std::cout << GREEN << "File edited.\n" << RESET;
            log_action(current_user, "Edited file: " + file);
        } else {
            std::cout << YELLOW << "Usage: edit <file> <content...>\n" << RESET;
        }
    } else if (cmd == "rm") {
        std::string file;
        ss >> file;
        if (!file.empty()) {
            if (delete_file(file)) {
                std::cout << GREEN << "File deleted.\n" << RESET;
                log_action(current_user, "Deleted file: " + file);
            } else {
                std::cout << RED << "File not found.\n" << RESET;
            }
        } else {
            std::cout << YELLOW << "Usage: rm <file>\n" << RESET;
        }
    } else if (cmd == "find") {
        std::string q;
        ss >> q;
        if (!q.empty()) {
            auto res = search_files(q);
            for (const auto& r : res) std::cout << CYAN << r << RESET << "\n";
            log_action(current_user, "Searched for: " + q);
        } else {
            std::cout << YELLOW << "Usage: find <query>\n" << RESET;
        }
    } else if (cmd == "calc") {
        calculator();
    } else if (cmd == "fsinfo") {
        auto items = list_directory("");
        std::cout << BOLDCYAN << "--- Virtual File System (VFS) Info ---\n" << RESET;
        std::cout << "Total Encrypted Files: " << items.size() << "\n";
        std::cout << "Storage Directory: /VFS_Storage\n";
        std::cout << "Encryption: XOR stream cipher\n";
        log_action(current_user, "Viewed fsinfo");
    } else if (cmd == "mem") {
#ifdef _WIN32
        PROCESS_MEMORY_COUNTERS_EX pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
            std::cout << BOLDCYAN << "--- OS Memory Usage Monitoring ---\n" << RESET;
            std::cout << "Private RAM Usage: " << (pmc.PrivateUsage / 1024) << " KB\n";
            std::cout << "Working Set Size: " << (pmc.WorkingSetSize / 1024) << " KB\n";
        }
        
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo)) {
            std::cout << "Total System RAM: " << (memInfo.ullTotalPhys / (1024*1024)) << " MB\n";
            std::cout << "Free System RAM: " << (memInfo.ullAvailPhys / (1024*1024)) << " MB\n";
        }
        log_action(current_user, "Checked memory usage");
#endif
    } else if (cmd == "ps") {
#ifdef _WIN32
        std::cout << BOLDCYAN << "--- Running Processes ---\n" << RESET;
        std::cout << "PID\tThreads\tExecutable\n";
        std::cout << "-----------------------------------\n";
        
        HANDLE hProcessSnap;
        PROCESSENTRY32 pe32;
        hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap != INVALID_HANDLE_VALUE) {
            pe32.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(hProcessSnap, &pe32)) {
                int count = 0;
                do {
                    std::cout << pe32.th32ProcessID << "\t" << pe32.cntThreads << "\t" << pe32.szExeFile << "\n";
                    if (++count > 15) {
                        std::cout << "... (truncated for readability)\n";
                        break;
                    }
                } while (Process32Next(hProcessSnap, &pe32));
            }
            CloseHandle(hProcessSnap);
        }
        log_action(current_user, "Listed processes");
#endif
    } else if (cmd == "spawn") {
#ifdef _WIN32
        std::string app;
        ss >> app;
        if (!app.empty()) {
            STARTUPINFOA si;
            PROCESS_INFORMATION pi;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));

            if (CreateProcessA(NULL, (LPSTR)app.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                std::cout << GREEN << "Successfully spawned process: " << app << " (PID: " << pi.dwProcessId << ")\n" << RESET;
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                log_action(current_user, "Spawned process: " + app);
            } else {
                std::cout << RED << "Failed to spawn process. Ensure the executable is in your PATH.\n" << RESET;
            }
        } else {
            std::cout << YELLOW << "Usage: spawn <executable_name>\n" << RESET;
        }
#endif
    } else if (cmd == "kill") {
#ifdef _WIN32
        int pid;
        if (ss >> pid) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
            if (hProcess != NULL) {
                if (TerminateProcess(hProcess, 0)) {
                    std::cout << GREEN << "Successfully killed process " << pid << "\n" << RESET;
                    log_action(current_user, "Killed process " + std::to_string(pid));
                } else {
                    std::cout << RED << "Failed to kill process.\n" << RESET;
                }
                CloseHandle(hProcess);
            } else {
                std::cout << RED << "Could not open process. You may need Administrator privileges.\n" << RESET;
            }
        } else {
            std::cout << YELLOW << "Usage: kill <pid>\n" << RESET;
        }
#endif
    } else if (cmd == "bgjob") {
        std::cout << MAGENTA << "Starting a background calculation thread...\n" << RESET;
        std::thread([]() {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cout << "\n" << BOLDGREEN << "[Background Thread] Finished heavy calculation!\n" << RESET;
            std::cout << BOLDGREEN << "admin@BambolinoOS:/$ " << RESET; // reprint prompt roughly
        }).detach();
        log_action(current_user, "Started background thread");
    } else if (cmd == "syscall") {
#ifdef _WIN32
        std::cout << BOLDCYAN << "--- Low-level System Call Demonstration ---\n" << RESET;
        std::cout << "Calling GetSystemTime()...\n";
        SYSTEMTIME st;
        GetSystemTime(&st);
        std::cout << "Kernel Time: " << st.wYear << "-" << st.wMonth << "-" << st.wDay << " " 
                  << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "\n\n";
                  
        std::cout << "Calling GetDiskFreeSpaceExA()...\n";
        ULARGE_INTEGER freeBytes, totalBytes, totalFreeBytes;
        if (GetDiskFreeSpaceExA("C:\\", &freeBytes, &totalBytes, &totalFreeBytes)) {
            std::cout << "C:\\ Drive Free Space: " << (freeBytes.QuadPart / (1024*1024*1024)) << " GB\n";
        }
        log_action(current_user, "Executed syscall demonstration");
#endif
    } else if (cmd == "showcase") {
        std::cout << BOLDCYAN << "\n=============================================\n";
        std::cout << "        OS LAB CONCEPTS SHOWCASE\n";
        std::cout << "=============================================\n" << RESET;
        
        std::cout << BOLDYELLOW << "1. Command Interpreter\n" << RESET;
        std::cout << "   " << GREEN << "Implemented in:" << RESET << " src/main.cpp (process_command loop)\n";
        std::cout << "   " << "Demonstration:" << " You are using it right now! It parses 'showcase' and maps it to execution logic.\n\n";
        
        std::cout << BOLDYELLOW << "2. File Management\n" << RESET;
        std::cout << "   " << GREEN << "Implemented in:" << RESET << " src/filesystem.cpp\n";
        std::cout << "   " << "Demonstration:" << " Type 'fsinfo', 'touch', 'ls', or 'rm' to interact with the Virtual File System.\n\n";
        
        std::cout << BOLDYELLOW << "3. Memory Usage Monitoring\n" << RESET;
        std::cout << "   " << GREEN << "Implemented in:" << RESET << " src/main.cpp (mem command)\n";
        std::cout << "   " << "Demonstration:" << " Type 'mem' to see live process PrivateUsage via GetProcessMemoryInfo().\n\n";
        
        std::cout << BOLDYELLOW << "4. Process Management\n" << RESET;
        std::cout << "   " << GREEN << "Implemented in:" << RESET << " src/main.cpp (ps, spawn, kill commands)\n";
        std::cout << "   " << "Demonstration:" << " Type 'spawn notepad' to spawn a child process via CreateProcess().\n\n";
        
        std::cout << BOLDYELLOW << "5. Multithreading\n" << RESET;
        std::cout << "   " << GREEN << "Implemented in:" << RESET << " src/main.cpp (bgjob command)\n";
        std::cout << "   " << "Demonstration:" << " Type 'bgjob' to spawn a detached std::thread while keeping the shell unblocked.\n\n";
        
        std::cout << BOLDYELLOW << "6. System Calls\n" << RESET;
        std::cout << "   " << GREEN << "Implemented in:" << RESET << " Win32 API calls across the project.\n";
        std::cout << "   " << "Demonstration:" << " Type 'syscall' to trigger direct GetSystemTime() kernel calls.\n";
        std::cout << BOLDCYAN << "=============================================\n\n" << RESET;
    } else if (cmd == "adduser") {
        if (current_role == "admin") {
            std::string nu, np, nr;
            std::cout << "New username: "; std::getline(std::cin, nu);
            std::cout << "New password: "; std::getline(std::cin, np);
            std::cout << "Role (admin/user): "; std::getline(std::cin, nr);
            if (add_user(current_role, nu, np, nr)) {
                std::cout << GREEN << "User added successfully.\n" << RESET;
                log_action(current_user, "Added new user: " + nu);
            }
        } else {
            std::cout << RED << "Access denied. Admins only.\n" << RESET;
            log_action(current_user, "Failed attempt to add user (Insufficient Privileges)");
        }
    } else if (cmd == "logs") {
        if (current_role == "admin") {
            auto logs = get_admin_logs();
            std::cout << BOLDCYAN << "\n--- ADMIN ACTIVITY LOGS ---\n" << RESET;
            for (const auto& l : logs) {
                std::cout << YELLOW << l << RESET << "\n";
            }
            std::cout << BOLDCYAN << "---------------------------\n\n" << RESET;
            log_action(current_user, "Viewed admin logs");
        } else {
            std::cout << RED << "Access denied. Admins only.\n" << RESET;
            log_action(current_user, "Failed attempt to view logs (Insufficient Privileges)");
        }
    } else if (cmd == "bambolino") {
        std::string prompt;
        std::getline(ss, prompt);
        if (prompt.empty()) {
            std::cout << YELLOW << "Usage: bambolino <your request>\n" << RESET;
        } else {
            std::cout << YELLOW << "[Bambolino] Thinking...\n" << RESET;
            std::string ai_response = chat_with_ai(prompt);
            
            if (ai_response.find("ERROR") != std::string::npos) {
                std::cout << RED << ai_response << RESET << "\n";
            } else {
                // Parse for <CMD> tags
                size_t cmd_start = ai_response.find("<CMD>");
                size_t cmd_end = ai_response.find("</CMD>");
                
                std::string chat_text = ai_response;
                std::string extracted_cmd = "";
                
                if (cmd_start != std::string::npos && cmd_end != std::string::npos && cmd_end > cmd_start) {
                    chat_text = ai_response.substr(0, cmd_start);
                    extracted_cmd = ai_response.substr(cmd_start + 5, cmd_end - (cmd_start + 5));
                }
                
                // Print the AI's conversational response wrapped neatly
                std::cout << BOLDMAGENTA << "[Bambolino] " << BOLDCYAN << wrap_text(chat_text) << RESET << "\n";
                
                // Execute the command if it provided one
                if (!extracted_cmd.empty()) {
                    std::cout << GREEN << "[System] Bambolino is executing: " << extracted_cmd << RESET << "\n";
                    process_command(extracted_cmd);
                }
            }
        }
    } else if (cmd == "help") {
        print_help();
    } else if (cmd == "logout" || cmd == "exit") {
        std::cout << "Logging out...\n";
        log_action(current_user, "Logged out");
        running = false;
    } else {
        std::cout << RED << "Command not found: " << cmd << ". Type 'help' for a list of commands.\n" << RESET;
    }

#ifdef _WIN32
    // Play the success sound effect after a command finishes
    PlaySoundA("faah.wav", NULL, SND_FILENAME | SND_ASYNC);
#endif
}

int main() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    init_fs();
    initialize_auth();

    std::cout << BOLDMAGENTA << "========================================\n";
    std::cout << "       BAMBOLINO OPERATING SYSTEM       \n";
    std::cout << "========================================\n\n" << RESET;

    // Login loop
    while (current_role.empty()) {
        std::string user, pass;
        std::cout << BOLDWHITE << "Username: " << RESET;
        std::getline(std::cin, user);
        std::cout << BOLDWHITE << "Password: " << RESET;
        std::getline(std::cin, pass);

        current_role = authenticate(user, pass);
        if (current_role.empty()) {
            std::cout << BOLDRED << "Invalid credentials. Try again.\n\n" << RESET;
            log_action(user, "FAILED login attempt");
        } else {
            current_user = user;
            std::cout << "\n" << BOLDGREEN << "Welcome, " << current_user << " (" << current_role << ")!\n" << RESET;
            log_action(current_user, "Successful login");
        }
    }

    // Command shell loop
    while (running) {
        std::cout << BOLDGREEN << current_user << BOLDBLUE << "@BambolinoOS" << BOLDWHITE << ":/$ " << RESET;
        std::string line;
        if (!std::getline(std::cin, line)) break;
        
        process_command(line);
    }

    return 0;
}
