@echo off
cls

:: 1. Check if g++ is installed
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] g++ compiler could not be found!
    echo Please ensure MinGW or MSYS2 is installed and added to your System PATH.
    exit /b 1
)

:: 2. Start Build Process
echo ========================================
echo [INFO] Starting Windows Build...
echo [INFO] Compiling main and sub-modules...
echo ========================================

:: 3. Compile and capture the result
g++ main.cpp 2048/2048.cpp NN/ai_tools.cpp -o app.exe -Wall -Wextra

:: 4. Check if compilation was successful
if %errorlevel% eq 0 (
    echo.
    echo [SUCCESS] Build completed! Executable: app.exe
    echo.
    
    set /p run_app="Run the app now? (y/n): "
    if /i "%run_app%"=="y" (
        echo --- Running app.exe ---
        app.exe
    )
) else (
    echo.
    echo [FATAL ERROR] Build failed. Check the compiler logs above.
    echo.
    exit /b 1
)