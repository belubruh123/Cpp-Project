#if FALSE
#include <bits/stdc++.h>
using namespace std;

vector<string> validGuesses;
vector<string> answers;

void loadData(){
    ifstream file("validGuesses.txt");
    string temp;
    while(file>>temp){
        validGuesses.push_back(temp);
    }
    file.close();
    ifstream file2("answers.txt");
    while(file2>>temp){
        answers.push_back(temp);
    }
    file2.close();
    sort(validGuesses.begin(), validGuesses.end());
}

string selectLanguage(){
    while(true){
        string s;
        cout<<"Please select a language, English(e) or Chinesec(c)";
        cin>>s;
        if(s=="e" || s=="c"){
            return s;
        }
    }
}

int hints(string ans,int hint){
    if(hint<=3){
        hint++;
        string s;
        cout<<"Do you need a hint?(y/n)";
        cin>>s;
        if(s=="y"){
            cout<<"The answer contains letter "<<ans[hint]<<"."<<endl;
        }
    }
    return hint;
}

string getRandomWord() {
    if (answers.empty()) return "";

    // 1. 建立隨機數產生器 (使用 random_device 當種子)
    static random_device rd;
    static mt19937 gen(rd());

    // 2. 定義範圍：從 0 到 vector 的最後一個索引
    uniform_int_distribution<> dis(0, answers.size() - 1);

    // 3. 取得隨機索引並回傳單字
    int randomIndex = dis(gen);
    return answers[randomIndex];
}

void intro(string s){
    if(s=="e"){
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

string input(int attempt){
    while(true){
        string s;
        cout<<"Please answer your guess, you have "<<attempt<<" attempts left."<<endl;
        cin>>s;
        if(s.length()==5 && binary_search(validGuesses.begin(), validGuesses.end(), s)){
            for (int i=0;i<s.length();i++) {
                s[i]=toupper(s[i]);
            }
            return s;
        }
        else{
            cout<<"Your guess must be a 5-letter English word";
        }
    }
}

bool wordle(){
    string language=selectLanguage();
    loadData();
    intro(language);
    string answer=getRandomWord();
    vector<vector<string>> output(6,vector<string>(5,"_"));
    const string RESET  = "\x1b[0m";
    const string GREEN  = "\x1b[32m";
    const string YELLOW = "\x1b[33m";
    const string GRAY   = "\x1b[90m";
    int chance=6,hint=0;
    while(chance>0){
        hint=hints(answer,hint);
        string guess=input(chance);
        for (int i=0;i<5;i++){
            if(guess[i]==answer[i]){
                output[6-chance][i]=GREEN+guess[i]+RESET;
                continue;
            }
            bool in=false;
            for (int j=0;j<5;j++){
                if(answer[j]==guess[i]){
                    output[6-chance][i]=YELLOW+guess[i]+RESET;
                    in=true;
                    break;
                }
            }
            if (!in) {
                output[6-chance][i]=GRAY+guess[i]+RESET;
            }
        }
        cout << "\033[2J\033[1;1H";
        intro(language);
        cout<<"============="<<endl;
        for(int i=0;i<6;i++){
            cout<<"|";
            for(int j=0;j<5;j++){
                cout<<" "<<output[i][j];
            }
            cout<<" |"<<endl;
        }
        cout<<"============="<<endl;
        cout<<"Nice Try!"<<endl;
        chance--;
        if(guess==answer){
            cout<<"Congrats! You guessed the correct word!"<<endl;
            cout<<"Play Again?(y/n)";
            string s;
            cin>>s;
            if (s=="y") {
                return true;
            }
            else {
                return false;
            }
        }
        else if(chance==0){
            cout<<"Better luck next time."<<endl;
            cout<<"Play Again?(y/n)";
            string s;
            cin>>s;
            if (s=="y") {
                return true;
            }
            else {
                return false;
            }
        }
    }
}
#endif