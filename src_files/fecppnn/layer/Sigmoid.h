

#ifndef KOIVISTO_RELU_H
#define KOIVISTO_RELU_H

#include "Layer.h"

#include <cmath>
class Sigmoid : public Layer {
    public:
    Sigmoid(Layer* prevLayer) : Layer(prevLayer->getOutput()->getSize()) { this->connect(prevLayer); };

    void compute() override {
        float* outputVals = getOutput()->getValues();
        float* inputVals  = getInput()->getValues();

        for (int i = 0; i < getOutput()->getSize(); i++) {
            outputVals[i] = 1.0 / (1+exp(-inputVals[i]));
        }
    }
    void backprop() override {

        float* outputGrads = getOutput()->getGradient()->getValues();
        float* outputVals  = getOutput()->getValues();
        float* inputGrads  = getInput()->getGradient()->getValues();

        for (int i = 0; i < getOutput()->getSize(); i++) {
            inputGrads[i] = outputGrads[i] * (outputVals[i] * (1 - outputVals[i]));
        }
    }
};

#endif    // KOIVISTO_RELU_H
