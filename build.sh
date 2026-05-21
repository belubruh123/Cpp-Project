#!/bin/bash

# Define some colors for the terminal output
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 1. Check if g++ is installed
if ! command -v g++ &> /dev/null; then
    echo -e "${RED}[ERROR] g++ compiler could not be found!${NC}"
    echo "Please install it using:"
    echo "   Mac:   xcode-select --install"
    echo "   Linux: sudo apt install g++"
    exit 1
fi

# 2. Start Build Process
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}[INFO] Starting Mac/Linux Build...${NC}"
echo -e "${BLUE}[INFO] Compiling main and sub-modules...${NC}"
echo -e "${BLUE}========================================${NC}"

# 3. Compile and capture the result (including relative paths)
g++ main.cpp 2048/2048.cpp NN/ai_tools.cpp -o app.out -Wall -Wextra

# 4. Check if compilation was successful
if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}[SUCCESS] Build completed! Executable: app.out${NC}"
    echo ""
    
    # Optional: Ask to run it immediately
    read -p "Run the app now? (y/n): " run_app
    if [[ "$run_app" == "y" || "$run_app" == "Y" ]]; then
        echo "--- Running app.out ---"
        ./app.out
    fi
else
    echo ""
    echo -e "${RED}[FATAL ERROR] Build failed. Check the compiler logs above.${NC}"
    echo ""
    exit 1
fi