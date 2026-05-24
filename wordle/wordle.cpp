#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>
#include "wordle.hpp"

using namespace std;

// 宣告全域變數
vector<string> validGuesses;
vector<string> answers;

// 1. 載入資料庫
void loadData(){
    validGuesses.clear();
    answers.clear();
    
    ifstream file("validGuesses.txt");
    string temp;
    while(file >> temp){
        for(auto &c : temp) c = toupper(c);
        validGuesses.push_back(temp);
    }
    file.close();
    
    ifstream file2("answers.txt");
    while(file2 >> temp){
        for(auto &c : temp) c = toupper(c);
        answers.push_back(temp);
    }
    file2.close();
    sort(validGuesses.begin(), validGuesses.end());
}

// 2. 選擇語言
string selectLanguage(){
    while(true){
        string s;
        cout << "Please select a language, English(e) or Chinese(c): ";
        cin >> s;
        if(s == "e" || s == "c"){
            return s;
        }
    }
}

// 3. 提示系統
int hints(string ans, int hint){
    if(hint <= 4){ 
        string s;
        cout << "Do you need a hint?(y/n): ";
        cin >> s;
        if(s == "y"){
            cout << "The answer contains letter " << ans[hint] << "." << endl;
            hint++; 
        }
    }
    return hint;
}

// 4. 取得隨機單字
string getRandomWord() {
    if (answers.empty()) return "";
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(0, answers.size() - 1);
    return answers[dis(gen)];
}

// 5. 遊戲說明畫面
void intro(string s){
    if(s == "e"){
        cout << R"(
          =============================================
                     WELCOME TO WORDLE C++
          =============================================
            - Guess the 5-letter word in 6 tries.
            - Each guess must be in the word list.
            - Feedback will be given for each letter.
          =============================================
        )" << endl;
    }
    else{
        cout << "=====================================" << endl;
        cout << "      WELCOME TO WORDLE (中文版)      " << endl;
        cout << "=====================================" << endl;
        cout << " 1. 目標：猜出隱藏的 5 字母單字" << endl;
        cout << " 2. 機會：共有 6 次嘗試機會" << endl;
        cout << " 3. 提示：綠色(正確)、黃色(錯位)、灰色(無)" << endl;
        cout << "-------------------------------------" << endl;
        cout << "[系統] 已就緒，請開始輸入單字。" << endl;
    }
}

// 6. 處理玩家輸入
string input(int attempt){
    while(true){
        string s;
        cout << "Please answer your guess, you have " << attempt << " attempts left." << endl;
        cin >> s;
        
        for (size_t i = 0; i < s.length(); i++) {
            s[i] = toupper(s[i]);
        }
        
        if(s.length() == 5 && binary_search(validGuesses.begin(), validGuesses.end(), s)){
            return s;
        }
        else{
            cout << "Your guess must be a valid 5-letter English word!" << endl;
        }
    }
}

// 7. 單局 Wordle 主核心
void wordleF(){
    string language = selectLanguage();
    loadData();
    
    if(answers.empty() || validGuesses.empty()) {
        cout << "[Error] Cannot find validGuesses.txt or answers.txt!" << endl;
        return;
    }
    
    intro(language);
    string answer = getRandomWord();
    vector<vector<string>> output(6, vector<string>(5, " _ "));
    vector<string> alphabet(26);
    for (int i = 0; i < 26; i++) {
        alphabet[i] = string(1, 'A' + i); 
    }
    const string reset  = "\x1b[0m";
    const string green  = "\x1b[42;30m";
    const string yellow = "\x1b[43;30m";
    const string gray   = "\x1b[100;30m"; 
    int chance = 6, hint = 0;
    
    while(chance > 0){
        hint = hints(answer, hint);
        string guess = input(chance);
        
        for (int i = 0; i < 5; i++){
            int idx = guess[i] - 'A';
            
            if(guess[i] == answer[i]){
                output[6-chance][i] = green + " "+ reset + green + guess[i] + reset + green + " "+ reset;
                alphabet[idx] = green + " "+ reset + green + guess[i] + reset + green + " "+ reset;
                continue;
            }
            
            bool in = false;
            for (int j = 0; j < 5; j++){
                if(answer[j] == guess[i]){
                    output[6-chance][i] = yellow + " "+ reset + yellow + guess[i] + reset + yellow + " "+ reset;
                    if (alphabet[idx].find("\x1b[32m") == string::npos) {
                        alphabet[idx] = yellow + " "+ reset + yellow + guess[i] + reset + yellow + " "+ reset;
                    }
                    in = true;
                    break;
                }
            }
            
            if (!in) {
                output[6-chance][i] = gray + " "+ reset + gray + guess[i] + reset + gray + " "+ reset;
                if (alphabet[idx].find("\x1b[3") == string::npos) {
                    alphabet[idx] = gray + " "+ reset + gray + guess[i] + reset + gray + " "+ reset;
                }
            }
        }
        
        cout << "\033[2J\033[1;1H" << flush;
        intro(language);
        
        cout << "=================" << endl;
        for(int i = 0; i < 6; i++){
            cout << "|";
            for(int j = 0; j < 5; j++){
                cout  << output[i][j];
            }
            cout << "|" << endl;
        }
        cout << "=================" << endl;
        
        for(int i = 0; i < 26; i++){
            cout << alphabet[i] << " ";
            if(i == 12) cout << endl; 
        }
        cout << "\n=================" << endl;
        
        cout << "Nice Try!" << endl;
        chance--;
        
        if(guess == answer){
            cout << "Congrats! You guessed the correct word!" << endl;
            break; 
        }
        else if(chance == 0){
            cout << "Better luck next time. The answer was: " << answer << endl;
            break; 
        }
    }
}

// 8. 重複遊玩外殼（供外部呼叫的公開介面）
void wordle(){
    while(true){
        wordleF();
        cout << "Play Again?(y/n): ";
        string s;
        cin >> s;
        if(s == "y" || s == "Y"){
            cout << "\033[2J\033[1;1H" << flush;
            continue;
        }
        else{
            break;
        }
    }
}
