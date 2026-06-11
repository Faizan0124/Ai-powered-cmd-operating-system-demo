@echo off
echo Compiling Bambolino OS with g++...
g++ src\main.cpp src\auth.cpp src\crypto_utils.cpp src\filesystem.cpp src\logger.cpp src\ai_bot.cpp -o BambolinoOS.exe -std=c++17 -lpsapi -lwinmm
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Compilation failed!
    pause
    exit /b %errorlevel%
)
echo.
echo [SUCCESS] Compilation successful!
echo You can now run the OS by typing: SecureOS.exe
pause
