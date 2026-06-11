cmake_minimum_required(VERSION 3.10)
project(BambolinoOS)

# Compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files
set(SOURCES
    src/main.cpp
    src/auth.cpp
    src/crypto_utils.cpp
    src/filesystem.cpp
    src/logger.cpp
    src/ai_bot.cpp
)

# Executable
add_executable(BambolinoOS ${SOURCES})
