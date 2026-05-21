@echo off
cls
color 0A

echo =========================================================
echo               C++ PROJECT BUILD SYSTEM
echo =========================================================
echo.

:: 1. Check if g++ compiler is available in PATH
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    color 0C
    echo [ERROR] g++ compiler could not be found!
    echo Please ensure MinGW/Dev-C++ is installed and added to your System PATH.
    echo.
    pause
    exit /b 1
)

:: 2. Clean up previous build if it exists
if exist game.exe (
    echo [INFO] Removing old game.exe...
    del game.exe
)

:: 3. Start Compilation Process
echo [INFO] Compiling modules with C++17 standard...
echo ---------------------------------------------------------

:: Added your "shut up" flags here (-D_HAS_STD_BYTE=0 and -w)
g++ -std=c++14 -w main.cpp wordle/wordle.cpp 2048/2048.cpp -o game.exe

:: 4. Check if compilation succeeded
if %errorlevel% equ 0 (
    echo ---------------------------------------------------------
    echo [SUCCESS] Build completed successfully! 
    echo [OUTPUT] Executable generated: game.exe
    echo.
    
    :: Ask user if they want to run the program immediately
    set /p run_app="Run the application now? (y/n): "
    if /i "%run_app%"=="y" (
        echo.
        echo Launching game.exe...
        echo ---------------------------------------------------------
        game.exe
    )
) else (
    color 0C
    echo ---------------------------------------------------------
    echo [FATAL ERROR] Build failed! 
    echo Please check the compiler error logs listed above.
    echo ---------------------------------------------------------
    echo.
    pause
)