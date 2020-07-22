//
// Created by finne on 7/22/2020.
//

#include <cmath>
#include "DenseOutput.h"



DenseOutput::DenseOutput(int inputSize) : inputSize(inputSize){
    
//    input = new float[inputSize];
    output = 1;
    weights = new float[inputSize];
    bias = 1;
    
//    for(int i = 0; i < outputSize; i++){
//        weights[i] = new float[inputSize];
//    }
    
    initWeights();
}

void DenseOutput::initWeights() {
    
    float bound = 1.0f / sqrt(inputSize);
    bias = bb::randDouble(-bound, bound);
    
    for (int n = 0; n < inputSize; n++) {
        weights[n] = bb::randDouble(-bound, bound);
        
    }
}



float DenseOutput::getOutput() const {
    return output;
}


float *DenseOutput::getWeights() const {
    return weights;
}

int DenseOutput::getInputSize() const {
    return inputSize;
}

int DenseOutput::getOutputSize() const {
    return 1;
}

float DenseOutput::getBias() const {
    return bias;
}

void DenseOutput::setBias(float bias) {
    DenseOutput::bias = bias;
}


void DenseOutput::setInput(float *input) {
//    delete DenseOutput::input;
    DenseOutput::input = input;
}

float *DenseOutput::getInput() const {
    return input;
}

float DenseOutput::getErrorSignal() const {
    return errorSignal;
}




