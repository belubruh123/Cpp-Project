#include <iostream>
#include <vector>
#include "ai_tools.hpp"
using namespace std;
extern int randint(int min, int max);
extern void delay(int ms);

void start_nn_demo() {
    //This is the demo for it, f(x) = 3x
    neuron my_demo(1,0.01);
    vector<float> trainData = {3,5,7,9,1,2,6};
    int epo;
    cout << "\n\n";
    cout << "Enter training rounds: ";
    cin >> epo;
    for (int i = 0; i < epo; i++) {
        for (int j = 0; j<(int)trainData.size(); j++) {
            int y = trainData[j]*3;
            my_demo.setInput({trainData[j]});
            cout << "Current Answer: " << my_demo.y_hat(false) << "  Correct Answer: " << y << endl;
            my_demo.train(2*(my_demo.y_hat(false)-y));
        }
    }
    //Final test
    int data = randint(1,100);
    my_demo.setInput({(float)data});
    cout << endl << "Current Answer: " << my_demo.y_hat(false) << " Correct Answer: " << data*3 << endl;
    delay(5000);
}
