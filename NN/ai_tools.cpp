#include <bits/stdc++.h>
using namespace std;
#pragma once
extern int randint(int min, int max);
extern void delay(int ms);

class neuron {
private:
    vector<float> weights;
    vector<float>input;
    float lr = 0.01;
    float bias = randint(10,1000)/100.0f;
public:
    neuron(int num_of_weight,float lrate) {
        weights.resize(num_of_weight);
        input.resize(num_of_weight);
        lr = lrate;
        for (int i = 0; i< (int)weights.size(); i++) {
            weights[i] = randint(10, 1000) / 100.0f;
        }
    }
    void setInput(const vector<float> &inp) {
        input = inp;
    }
    float y_hat(bool relu) {
        float ans = 0;
            for (int i = 0; i<(int)weights.size();i++) {
                ans += weights[i]*input[i];
            }
        ans += bias; //Quick reminder, I didnt learn how to train bias actually but I think it is the same as weight but input/x is treated as 1
        return ( relu ? (ans < 0? 0 : ans) : ans );
    }
    void train(float grad_before) {
        for (int i = 0; i<(int)weights.size(); i++) {
            weights[i] -= (lr*grad_before*input[i]);
        }
        bias -= (lr*grad_before);
    }
    vector<float> get_weight() {
        return weights;
    }
};

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
