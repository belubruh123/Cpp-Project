#include <bits/stdc++.h>
#include <functional>
#include "2048/2048.hpp"
#include "NN/ai_tools.cpp"
#include "wordle/wordle.cpp"

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    int getch() {
        //Can understand? Same to me (only know a bit but yeah thanks for our dear AI guy to modify the Linux input to not wait for enter)
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

using namespace std;

#define RESET        "\033[0m"
#define TEXT_RED     "\033[31m"
#define TEXT_WHITE   "\033[37m"
#define BOLD_RED     "\033[1;31m"
#define BOLD_WHITE   "\033[1;37m"
#define HIGHLIGHT    "\033[1;30;47m" 
#define BORDER_RED   "\033[1;31m"

void init_terminal() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | 0x0004); 
    }
#endif
}

void clearScreen() {
    cout << "\033[2J\033[1;1H"; 
}

class MainMenu {
private:
    struct MenuOption {
        string description;
        function<void()> callback;
    };
    vector<MenuOption> options;
    int selectedIndex = 0;

    void drawHeader() {
        //This ascii is converted by AI. Too lazy to type it out
        cout << BOLD_RED;
        cout << "      ______        " << BOLD_WHITE << "   _      _   " << endl;
        cout << BOLD_RED;
        cout << "    /  ____|       " << BOLD_WHITE << " _| |_  _| |_ " << endl;
        cout << BOLD_RED;
        cout << "   |  |            " << BOLD_WHITE << "|_   _||_   _|" << endl;
        cout << BOLD_RED;
        cout << "   |  |             " << BOLD_WHITE << " |_|    |_|  " << endl;
        cout << BOLD_RED;
        cout << "    \\  \\____       " << BOLD_WHITE << "              " << endl;
        cout << BOLD_RED;
        cout << "     \\______|      " << RESET << endl;
        
        cout << BORDER_RED << "=========================================================" << RESET << endl;
        cout << BOLD_WHITE << "  [↑/↓] 選擇  |  [Enter] 確認選擇" << RESET << endl;
        cout << BORDER_RED << "=========================================================" << RESET << endl << endl;
    }

public:
    void addOption(const string& description, function<void()> callback) {
        options.push_back({description, callback});
    }

    void show() {
        bool running = true;
        while (running) {
            clearScreen();
            drawHeader();

            for (size_t i = 0; i < options.size(); ++i) {
                if (i == selectedIndex) {
                    cout << BOLD_RED << "  ► " << HIGHLIGHT << " " << options[i].description << " " << RESET << endl;
                } else {
                    cout << TEXT_RED << "    ◦ " << TEXT_WHITE << options[i].description << RESET << endl;
                }
            }

            cout << endl << BORDER_RED << "=========================================================" << RESET << endl;

            int ch = getch();
            if (ch == 0 || ch == 224) { // Windows arrow keys
                ch = getch();
                if (ch == 72 && selectedIndex > 0) selectedIndex--;
                if (ch == 80 && selectedIndex < options.size() - 1) selectedIndex++;
            } 
            else if (ch == 27) { // Linux/macOS arrow keys
                ch = getch();
                if (ch == 91) {
                    ch = getch();
                    if (ch == 65 && selectedIndex > 0) selectedIndex--;
                    if (ch == 66 && selectedIndex < options.size() - 1) selectedIndex++;
                }
            } 
            else if (ch == 10 || ch == 13) { // Enter key
                clearScreen();
                options[selectedIndex].callback();
                
                if (selectedIndex == options.size() - 1) {
                    running = false;
                } else {
                    cout << TEXT_RED << "\n\n請按任意鍵返回主選單..." << RESET;
                    getch();
                }
            }
        }
    }
};

int main() {
    init_terminal();
    MainMenu menu;

    menu.addOption("2048 ", []() {
        play2048();
    });

    menu.addOption("Wordle", []() {
         wordle();
    });

    menu.addOption("神經網路模型訓練 ", []() {
        clearScreen();
        start_nn_demo();
    });

    menu.addOption("關閉系統並退出", []() {
        cout << BOLD_RED << "系統已關閉。再見。" << RESET << endl;
    });

    menu.show();

    return 0;
}