#include <bits/stdc++.h>
#include "../NN/ai_tools.cpp"
using namespace std;
#define CLEAR_SCREEN cout << "\033[2J\033[1;1H";
#define DARK_TXT "\033[38;2;119;110;101m" // Only 2 and 4
#define WHT_TXT "\033[38;2;249;246;242m"
#define RESET "\033[0m"

#ifdef _WIN32
    #include <conio.h>
#endif

extern int getch(); // Too lazy to make a header for main.cpp
random_device rd;
mt19937 gen(rd());
int maxTile = 2;
int board[4][4];
int current_free = 16; // Sorry for snake case. This is a bit too long for camel so I think snake would be better.
int canSpawn[16];
int get_score = 0;
string COLORS[] = {
    "\033[38;2;249;246;242m", // Ok I know you can use WHT_TXT but this is more neat
    "\033[48;2;238;228;218m",
    "\033[48;2;237;224;200m",
    "\033[48;2;242;177;121m",
    "\033[48;2;245;149;99m",
    "\033[48;2;246;124;95m",
    "\033[48;2;246;94;59m",
    "\033[48;2;237;207;114m",
    "\033[48;2;237;204;97m",
    "\033[48;2;237;200;80m",
    "\033[48;2;237;197;63m",
    "\033[48;2;237;194;46m",
    "\033[48;2;60;58;50m"};

int randint(int min, int max)
{
    uniform_int_distribution<> distr(min, max);
    return distr(gen);
}

void delay(int ms)
{
    this_thread::sleep_for(chrono::milliseconds(ms));
}

void printBoard()
{
    cout << "\n";
    for (int i = 0; i < 4; i++)
    {
        cout << "|";
        for (int j = 0; j < 4; j++)
        {
            int tileNum = board[i][j];
            int tile = tileNum ? (int)log2(tileNum) : 0; // CPU heavy isn't it?
            cout << COLORS[(tile > 12 ? 12 : tile)] << (tileNum < 8 ? DARK_TXT : WHT_TXT) << string(((to_string(maxTile).length() + 2) - (tileNum ? to_string(tileNum) : " ").length()) / 2, ' ') << (tileNum ? to_string(tileNum) : " ") << string((to_string(maxTile).length() + 2) - (tileNum ? to_string(tileNum) : " ").length() - (((to_string(maxTile).length() + 2) - (tileNum ? to_string(tileNum) : " ").length()) / 2), ' ') << RESET << "|";
        }
        cout << endl;
    }
}
void left()
{
    for (int i = 0; i < 4; i++)
    {
        int target = 0;
        for (int j = 0; j < 4; j++)
        {
            if (board[i][j] != 0)
            {
                swap(board[i][target], board[i][j]);
                target++;
            }
        }
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] != 0 && board[i][j] == board[i][j + 1])
            {
                int merged = board[i][j] *= 2;
                get_score += merged;
                if (merged > maxTile)
                    maxTile = merged;
                board[i][j + 1] = 0;
                current_free++;
            }
        }
        target = 0;
        for (int j = 0; j < 4; j++)
        {
            if (board[i][j] != 0)
            {
                swap(board[i][target], board[i][j]);
                target++;
            }
        }
    }
}

void right()
{
    for (int i = 0; i < 4; i++)
    {
        int target = 3;
        for (int j = 3; j >= 0; j--)
        {
            if (board[i][j] != 0)
            {
                swap(board[i][target], board[i][j]);
                target--;
            }
        }
        for (int j = 3; j > 0; j--)
        {
            if (board[i][j] != 0 && board[i][j] == board[i][j - 1])
            {
                int merged = board[i][j] *= 2;
                get_score += merged;
                if (merged > maxTile)
                    maxTile = merged;
                board[i][j - 1] = 0;
                current_free++;
            }
        }
        target = 3;
        for (int j = 3; j >= 0; j--)
        {
            if (board[i][j] != 0)
            {
                swap(board[i][target], board[i][j]);
                target--;
            }
        }
    }
}

void up()
{
    for (int j = 0; j < 4; j++)
    {
        int target = 0;
        for (int i = 0; i < 4; i++)
        {
            if (board[i][j] != 0)
            {
                swap(board[target][j], board[i][j]);
                target++;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            if (board[i][j] != 0 && board[i][j] == board[i + 1][j])
            {
                int merged = board[i][j] *= 2;
                get_score += merged;
                if (merged > maxTile)
                    maxTile = merged;
                board[i + 1][j] = 0;
                current_free++;
            }
        }
        target = 0;
        for (int i = 0; i < 4; i++)
        {
            if (board[i][j] != 0)
            {
                swap(board[target][j], board[i][j]);
                target++;
            }
        }
    }
}

void down()
{
    for (int j = 0; j < 4; j++)
    {
        int target = 3;
        for (int i = 3; i >= 0; i--)
        {
            if (board[i][j] != 0)
            {
                swap(board[target][j], board[i][j]);
                target--;
            }
        }
        for (int i = 3; i > 0; i--)
        {
            if (board[i][j] != 0 && board[i][j] == board[i - 1][j])
            {
                int merged = board[i][j] *= 2;
                get_score += merged;
                if (merged > maxTile)
                    maxTile = merged;
                board[i - 1][j] = 0;
                current_free++;
            }
        }
        target = 3;
        for (int i = 3; i >= 0; i--)
        {
            if (board[i][j] != 0)
            {
                swap(board[target][j], board[i][j]);
                target--;
            }
        }
    }
}

bool isDead()
{
    // Basically trying out all neighbors :P
    if (current_free)
        return false; // Cause you need it to be 0 which is false so it actually check the thing at down
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] == board[i][j + 1])
                return false;
        }
    }
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 3; i++)
        {
            if (board[i][j] == board[i + 1][j])
                return false;
        }
    }
    return true;
}

void spawn()
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (board[i][j] == 0)
            {
                canSpawn[count++] = i * 4 + j;
            }
        }
    }

    current_free = count;

    if (count > 0)
    {
        int choice = canSpawn[randint(0, count - 1)];
        board[choice / 4][choice % 4] = (randint(0, 9) == 0 ? 4 : 2);
        current_free--;
    }
}

// Trying to keep things modulize here :) which I dont really do
void init()
{
    for (int i = 0; i < 16; i++)
    {
        canSpawn[i] = 0;
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            board[i][j] = {0};
        }
    }
}

void play2048()
{
    init();
    spawn();
    int mode = 2;
    cout << "\n";
    cout << "Welcome to 2048! Enter 1 for AI training mode, 0 for human playing mode!" << endl;
    cout << "Your choice: ";
    while (mode > 1)
    {
        cin >> mode;
    }
    if (!mode)
    {
        int total_score = 0;
        while (true)
        {
            CLEAR_SCREEN
            if (isDead())
            { // aka if it is 0 it will return true
                delay(1000);
                cout << "輸了！" << "\n";
                delay(1000);
                break;
            }
            printBoard();
            cout << "\n Your Score:" << total_score;
            cout << endl
                 << "請選擇 (WASD 或 Q 離開)： ";
            char input = tolower(getch());
            // cin >> input;
            int before[4][4];
            copy(&board[0][0], &board[0][0] + 16, &before[0][0]);
            if (input == 'q')
                break;
            if (input != 'w' && input != 'a' && input != 's' && input != 'd')
                continue;
            if (input == 'w')
                up();
            if (input == 's')
                down();
            if (input == 'a')
                left();
            if (input == 'd')
                right();
            if (!equal(&board[0][0], &board[0][0] + 16, &before[0][0]))
                spawn();
            total_score += get_score;
            get_score = 0;
        }
    }
    else
    {
        CLEAR_SCREEN
        cout << "\nLooks like you want to train AI! Enter the rounds it should play: ";
        int rounds = 0;
        cin >> rounds;
        // We are going to make 4 NN each with 2 hidden layer and 1 final output layer
        int gamma = 0.95;
        /*Design: 256 input 16x16 each of the first 16 is paired with log2 of the tile index as 1
            and then after that first layer is going to be 512 neuron each with 256 input
            then after that we have 256 neuron with 512 input and finally layer with 4 neuron 256 input 
        */
        vector<neuron> h1(512,neuron(256,0.001f));
        vector<neuron> h2(256,neuron(512,0.001f));
        vector<neuron> finale(4,neuron(256,0.001f));

        for (int i = 0; i < rounds; i++)
        {
            int choice = 0;
            int total_score = 0;
            bool first = true;
            float reward = 0;
            bool stuck = false;
            current_free = 16;
            maxTile = 2;
            while (true)
            {
                CLEAR_SCREEN
                printBoard();
                if (isDead())
                { // aka if it is 0 it will return true
                    delay(1000);
                    cout << "輸了！" << "\n";
                    delay(1000);
                    init();
                    spawn();
                }
                vector<float> input(256,0);
                for (int i = 0; i<4; i++) {
                    for (int j = 0; j<4; j++) {
                        int index = 0;
                        if (board[i][j]) {
                            index = log2(board[i][j]);
                        }
                        input[(i * 4 + j) * 16 + index] = 1;
                    }

                }
                vector<float>h1_out(512);
                for (int i = 0; i<512; i++) {
                    h1[i].setInput(input);
                    h1_out[i] = h1[i].y_hat(true);
                }
                vector<float>h2_out(256);
                for (int i = 0; i<256; i++) {
                    h2[i].setInput(h1_out);
                    h2_out[i] = h2[i].y_hat(true);
                }
                vector<float>q_out(4);
                for (int i = 0; i<4; i++) {
                    finale[i].setInput(h2_out);
                    q_out[i]=finale[i].y_hat(false);
                }
                int before[4][4];
                copy(&board[0][0], &board[0][0] + 16, &before[0][0]);
                if (stuck || randint(1, 100) <= 10) { 
                    choice = randint(0, 3); // Force a random move to break the freeze
                } else {
                    choice = distance(q_out.begin(), max_element(q_out.begin(), q_out.end()));
                }
                if (choice == 0) {
                    up();
                } else if (choice == 1) {
                    down();
                } else if (choice == 2) {
                    left();
                } else if (choice == 3) {
                    right();
                }
                if (isDead())
                {
                    reward = -999;
                }
                reward = (!get_score ? -1 : reward + get_score);
                if (!equal(&board[0][0], &board[0][0] + 16, &before[0][0]))
                {
                    spawn();
                    stuck = false; 
                } else {
                    reward = -100;
                    stuck = true;
                }
                total_score += get_score;
                cout << "\n Score:" << total_score << endl;
                get_score = 0;
                //This is not a duplicate! I am making the training here!
                vector<float> inputp(256,0);
                for (int i = 0; i<4; i++) {
                    for (int j = 0; j<4; j++) {
                        int index = 0;
                        if (board[i][j]) {
                            index = log2(board[i][j]);
                        }
                        inputp[(i * 4 + j) * 16 + index] = 1;
                    }

                }
                vector<float>h1_outp(512);
                for (int i = 0; i<512; i++) {
                    h1[i].setInput(inputp);
                    h1_outp[i] = h1[i].y_hat(true);
                }
                vector<float>h2_outp(256);
                for (int i = 0; i<256; i++) {
                    h2[i].setInput(h1_outp);
                    h2_outp[i] = h2[i].y_hat(true);
                }
                vector<float>q_outp(4);
                for (int i = 0; i<4; i++) {
                    finale[i].setInput(h2_outp);
                    q_outp[i]=finale[i].y_hat(false);
                }
                float target = (float)reward + (gamma * (*max_element(q_outp.begin(), q_outp.end())));
                
                float grad_finale = (q_out[choice] - target) * 2;
                finale[choice].train(grad_finale);

                vector<float> finale_weights = finale[choice].get_weight();
                vector<float> grad_h2(256, 0.0f);

                for (int j = 0; j < 256; j++) {
                    if (h2_out[j] > 0) {
                        grad_h2[j] = grad_finale * finale_weights[j];
                        h2[j].setInput(h1_out);
                        h2[j].train(grad_h2[j]);
                    }
                }

                for (int i = 0; i < 512; i++) {
                    if (h1_out[i] > 0) {
                        float grad_h1_i = 0.0f;
                        for (int j = 0; j < 256; j++) {
                            if (h2_out[j] > 0) {
                                vector<float> h2_weights = h2[j].get_weight();
                                grad_h1_i += grad_h2[j] * h2_weights[i];
                            }
                        }
                        h1[i].setInput(input);
                        h1[i].train(grad_h1_i);
                    }
                }
                delay(50);
            }
        }
    }
}
