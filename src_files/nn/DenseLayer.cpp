//
// Created by finne on 7/22/2020.
//

#include <cmath>
#include "DenseLayer.h"



DenseLayer::DenseLayer(int inputSize, int outputSize) : inputSize(inputSize), outputSize(outputSize) {
    
    
    input = new float[inputSize];
    output = new float[outputSize];
    errorSignal = new float[outputSize];
    
    weights = new float[outputSize*inputSize];
    bias = new float[outputSize];
    
    initWeights();

}

void DenseLayer::initWeights() {
        
        float bound = 1.0f / sqrt(inputSize);
        
        
        for(int i = 0; i < outputSize; i++){
            bias[i] = bb::randDouble(-bound,bound);
            for(int n = 0; n < inputSize; n++){
                weights[i * inputSize + n] = bb::randDouble(-bound, bound);
            }
        }
    
}

float *DenseLayer::getInput() const {
    return input;
}

float *DenseLayer::getOutput() const {
    return output;
}

int DenseLayer::getInputSize() const {
    return inputSize;
}

int DenseLayer::getOutputSize() const {
    return outputSize;
}

float *DenseLayer::getWeights() const {
    return weights;
}

float *DenseLayer::getBias() const {
    return bias;
}

void DenseLayer::setInput(float *input) {
    delete DenseLayer::input;
    DenseLayer::input = input;
}

float *DenseLayer::getErrorSignal() const {
    return errorSignal;
}



