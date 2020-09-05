//
// Created by finne on 7/22/2020.
//

#include <cmath>
#include "DenseOutput.h"
#include "data/Weight.h"


nn::DenseOutput::DenseOutput(int inputSize) : inputSize(inputSize){
    
//    input = new float[inputSize];
    output = 1;
    weights = new Weight(inputSize);
    bias  = new Weight(1);
    
//    for(int i = 0; i < outputSize; i++){
//        weights[i] = new float[inputSize];
//    }
    
    initWeights();
}

void nn::DenseOutput::initWeights() {
    
    float bound = 1.0f / sqrt(inputSize);
    bias->value[0] = nn::randFloat(-bound, bound);
    
    for (int n = 0; n < inputSize; n++) {
        weights->value[n] = nn::randFloat(-bound, bound);
        
    }
}



float nn::DenseOutput::getOutput() const {
    return output;
}


Weight *nn::DenseOutput::getWeights() const {
    return weights;
}

int nn::DenseOutput::getInputSize() const {
    return inputSize;
}

int nn::DenseOutput::getOutputSize() const {
    return 1;
}

Weight* nn::DenseOutput::getBias() const {
    return bias;
}


void nn::DenseOutput::setInput(float *input) {
//    delete DenseOutput::input;
    DenseOutput::input = input;
}

float *nn::DenseOutput::getInput() const {
    return input;
}


void nn::DenseOutput::setBias(Weight *bias) {
    DenseOutput::bias = bias;
}

void nn::DenseOutput::setWeights(Weight *weights) {
    DenseOutput::weights = weights;
}

nn::DenseOutput::~DenseOutput() {
    delete weights;
    delete bias;
}




