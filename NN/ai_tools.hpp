#ifndef AI_TOOLS_HPP
#define AI_TOOLS_HPP

#include <vector>

extern int randint(int min, int max);

class neuron {
private:
    std::vector<float> weights;
    std::vector<float> input;
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
    void setInput(const std::vector<float> &inp) {
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
    std::vector<float> get_weight() {
        return weights;
    }
};

void start_nn_demo();

#endif
